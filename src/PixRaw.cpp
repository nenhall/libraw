#include "PixRaw.h"
#include "ImageAdjuster.h"
#include "RawData.h"
#include <algorithm>
#include <cstring>
#include <libraw/libraw.h>

namespace PixRaw {

// Pimpl 实现类
class PixRaw::Impl {
public:
  Impl() : libraw_(std::make_unique<LibRaw>()), open_(false) {}

  bool open(const std::string &filepath) {
    close(); // 这会清除缓存

#ifdef _WIN32
    // Windows: 宽字符
    int ret = libraw_->open_file(filepath.c_str());
#else
    int ret = libraw_->open_file(filepath.c_str());
#endif

    if (ret != LIBRAW_SUCCESS) {
      error_ = "Failed to open file: " + std::string(libraw_strerror(ret));
      return false;
    }

    open_ = true;
    error_.clear();
    return true;
  }

  bool open(const std::wstring &filepath) {
    close();

#ifdef _WIN32
    int ret = libraw_->open_file(filepath.c_str());
#else
    // 非 Windows 系统转换宽字符为 UTF-8
    // 简化处理，实际需要转换
    return false;
#endif

    if (ret != LIBRAW_SUCCESS) {
      error_ = "Failed to open file";
      return false;
    }

    open_ = true;
    error_.clear();
    return true;
  }

  RawMetadata getMetadata() const {
    RawMetadata metadata;

    if (!open_) {
      return metadata;
    }

    // 图像尺寸
    libraw_image_sizes_t &sizes = libraw_->imgdata.sizes;
    metadata.image_width = sizes.width;
    metadata.image_height = sizes.height;
    metadata.raw_width = sizes.raw_width;
    metadata.raw_height = sizes.raw_height;

    // 相机信息
    libraw_iparams_t &idata = libraw_->imgdata.idata;
    metadata.camera_make = idata.make;
    metadata.camera_model = idata.model;
    metadata.software = idata.software;

    // 拍摄参数
    libraw_imgother_t &other = libraw_->imgdata.other;
    metadata.iso = other.iso_speed;
    metadata.shutter_speed = other.shutter;
    metadata.aperture = other.aperture;
    metadata.focal_length = other.focal_len;
    metadata.timestamp = other.timestamp;

    // 白平衡
    libraw_colordata_t &color = libraw_->imgdata.color;
    metadata.wb_red = color.cam_mul[0];
    metadata.wb_green = color.cam_mul[1];
    metadata.wb_blue = color.cam_mul[2];

    return metadata;
  }

  RawImage decodePreview(int max_width, int max_height) {
    if (!open_) {
      error_ = "No file opened";
      return RawImage();
    }

    // 如果已经解码过，并且有缓存，直接使用缓存
    if (image_decoded_ && cached_image_.isValid()) {
      // 应用当前的调整参数
      if (adjustments_.hasAdjustments()) {
        return ImageAdjuster::applyAdjustments(cached_image_, adjustments_);
      }
      // 如果没有调整，返回缓存的副本
      RawImage result(cached_image_.width(), cached_image_.height(), cached_image_.format());
      int dataSize = cached_image_.width() * cached_image_.height() * cached_image_.bytesPerPixel();
      std::memcpy(result.data(), cached_image_.data(), dataSize);
      return result;
    }

    // 首次解码
    // 获取原始图像尺寸
    libraw_image_sizes_t &sizes = libraw_->imgdata.sizes;
    int original_width = sizes.width;
    int original_height = sizes.height;

    // 计算缩放比例，保持宽高比
    double scale = 1.0;
    if (max_width > 0 && max_height > 0) {
      double scale_width = static_cast<double>(max_width) / original_width;
      double scale_height = static_cast<double>(max_height) / original_height;
      scale = (scale_width < scale_height) ? scale_width : scale_height;

      // 如果图像比目标尺寸小，不放大
      if (scale > 1.0) {
        scale = 1.0;
      }
    }

    // 设置输出参数
    libraw_output_params_t &out_params = libraw_->imgdata.params;
    out_params.output_bps = 8;    // 8-bit per channel
    out_params.use_camera_wb = 1; // 使用相机白平衡
    out_params.use_auto_wb = 0;
    out_params.user_qual = 3;                     // AHD 算法，质量较好
    out_params.half_size = (scale < 0.6) ? 1 : 0; // 使用 LibRaw 的 half_size 选项

    // 解包
    int ret = libraw_->unpack();
    if (ret != LIBRAW_SUCCESS) {
      error_ = "Unpack failed: " + std::string(libraw_strerror(ret));
      return RawImage();
    }

    // 处理
    ret = libraw_->dcraw_process();
    if (ret != LIBRAW_SUCCESS) {
      error_ = "Process failed: " + std::string(libraw_strerror(ret));
      return RawImage();
    }

    // 获取图像
    libraw_processed_image_t *image = libraw_->dcraw_make_mem_image(&ret);
    if (!image) {
      error_ = "Failed to create image";
      return RawImage();
    }

    // 转换为 RawImage 并缓存
    if (image->type == LIBRAW_IMAGE_BITMAP && image->colors == 3) {
      int width = image->width;
      int height = image->height;
      int data_size = width * height * 3;

      cached_image_ = RawImage(width, height, PixelFormat::RGB888);

      // 确保数据被正确拷贝
      if (cached_image_.data() && image->data) {
        std::memcpy(cached_image_.data(), image->data, data_size);
        image_decoded_ = true;
      } else {
        error_ = "Failed to copy image data";
      }
    } else {
      error_ = "Unsupported image format";
      LibRaw::dcraw_clear_mem(image);
      return RawImage();
    }

    LibRaw::dcraw_clear_mem(image);

    // 应用调整参数
    if (cached_image_.isValid() && adjustments_.hasAdjustments()) {
      return ImageAdjuster::applyAdjustments(cached_image_, adjustments_);
    }

    // 返回缓存的副本
    RawImage result(cached_image_.width(), cached_image_.height(), cached_image_.format());
    int dataSize = cached_image_.width() * cached_image_.height() * cached_image_.bytesPerPixel();
    std::memcpy(result.data(), cached_image_.data(), dataSize);
    return result;
  }

  RawImage decodeFull() {
    return decodePreview(0, 0); // 全尺寸，不限制
  }

  RawImage decodeQuickPreview() {
    // 超快速预览：320x240（约 7万像素）
    // 使用去马赛克快速算法或下采样
    return decodePreview(320, 240);
  }

  RawImage decodeMediumPreview() {
    // 中等预览：1280x720（约 92万像素）
    // 平衡质量和速度
    return decodePreview(1280, 720);
  }

  RawImage getThumbnail() {
    if (!open_) {
      error_ = "No file opened";
      return RawImage();
    }

    int ret = libraw_->unpack_thumb();
    if (ret != LIBRAW_SUCCESS) {
      // 如果解包缩略图失败，尝试使用小尺寸预览
      // 某些相机可能没有嵌入缩略图
      return decodePreview(480, 480); // 返回小尺寸预览，最大边480
    }

    libraw_processed_image_t *thumb = libraw_->dcraw_make_mem_thumb(&ret);
    if (!thumb) {
      // 如果创建缩略图失败，使用小尺寸预览
      return decodePreview(480, 480);
    }

    RawImage result;
    if (thumb->type == LIBRAW_IMAGE_JPEG) {
      // JPEG 缩略图 - 使用 Qt 解码
      // 注意：这里假设 Qt 可用，在纯 C++ 环境需要其他 JPEG 库
      // 暂时返回空，让调用者使用 decodePreview
      LibRaw::dcraw_clear_mem(thumb);
      return decodePreview(480, 480); // 返回小尺寸快速预览
    } else if (thumb->type == LIBRAW_IMAGE_BITMAP && thumb->colors == 3) {
      int width = thumb->width;
      int height = thumb->height;
      int data_size = width * height * 3;

      result = RawImage(width, height, PixelFormat::RGB888);

      if (result.data() && thumb->data) {
        std::memcpy(result.data(), thumb->data, data_size);
      } else {
        error_ = "Failed to copy thumbnail data";
      }
    } else {
      error_ = "Unsupported thumbnail format";
    }

    LibRaw::dcraw_clear_mem(thumb);

    // 如果缩略图获取失败，使用小尺寸预览
    if (!result.isValid()) {
      return decodePreview(480, 480);
    }

    return result;
  }

  RawData getThumbnailData() {
    if (!open_) {
      error_ = "No file opened";
      return RawData();
    }

    int ret = libraw_->unpack_thumb();
    if (ret != LIBRAW_SUCCESS) {
      error_ = "Unpack thumbnail failed";
      return RawData();
    }

    libraw_processed_image_t *thumb = libraw_->dcraw_make_mem_thumb(&ret);
    if (!thumb) {
      error_ = "Failed to create thumbnail";
      return RawData();
    }

    // 只返回 JPEG 格式的数据
    if (thumb->type == LIBRAW_IMAGE_JPEG) {
      RawData result(thumb->data, thumb->data_size);
      LibRaw::dcraw_clear_mem(thumb);
      return result;
    } else {
      // 不是 JPEG 格式，返回空
      LibRaw::dcraw_clear_mem(thumb);
      error_ = "Thumbnail is not in JPEG format";
      return RawData();
    }
  }

  std::string getLastError() const { return error_; }

  bool isOpen() const { return open_; }

  void close() {
    if (open_) {
      libraw_->recycle();
      open_ = false;
      image_decoded_ = false;
      cached_image_ = RawImage(); // 释放缓存
    }
  }

  void setAdjustments(const RawAdjustments &adjustments) { adjustments_ = adjustments; }

  RawAdjustments getAdjustments() const { return adjustments_; }

private:
  std::unique_ptr<LibRaw> libraw_;
  bool open_;
  std::string error_;
  RawAdjustments adjustments_;
  RawImage cached_image_;      // 缓存已解码的原始图像
  bool image_decoded_ = false; // 是否已经解码过
};

// === PixRaw 实现 ===

PixRaw::PixRaw() : impl_(std::make_unique<Impl>()) {}

PixRaw::~PixRaw() { close(); }

PixRaw::PixRaw(PixRaw &&) noexcept = default;

PixRaw &PixRaw::operator=(PixRaw &&) noexcept = default;

bool PixRaw::open(const std::string &filepath) { return impl_->open(filepath); }

bool PixRaw::open(const std::wstring &filepath) { return impl_->open(filepath); }

RawMetadata PixRaw::getMetadata() const { return impl_->getMetadata(); }

RawImage PixRaw::decodePreview(int max_width, int max_height) { return impl_->decodePreview(max_width, max_height); }

RawImage PixRaw::decodeFull() { return impl_->decodeFull(); }

RawImage PixRaw::decodeQuickPreview() { return impl_->decodeQuickPreview(); }

RawImage PixRaw::decodeMediumPreview() { return impl_->decodeMediumPreview(); }

RawImage PixRaw::getThumbnail() { return impl_->getThumbnail(); }

std::string PixRaw::getLastError() const { return impl_->getLastError(); }

bool PixRaw::isOpen() const { return impl_->isOpen(); }

RawData PixRaw::getThumbnailData() { return impl_->getThumbnailData(); }

void PixRaw::close() { impl_->close(); }

void PixRaw::setAdjustments(const RawAdjustments &adjustments) { impl_->setAdjustments(adjustments); }

RawAdjustments PixRaw::getAdjustments() const { return impl_->getAdjustments(); }

} // namespace PixRaw
