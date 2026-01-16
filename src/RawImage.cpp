#include "RawImage.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace PixRaw {

// 辅助函数：计算每像素字节数
int bytesPerPixelForFormat(PixelFormat format) {
    switch (format) {
        case PixelFormat::RGB888:  return 3;
        case PixelFormat::RGBA8888: return 4;
        case PixelFormat::RGB565:  return 2;
    }
    return 3;
}

// === RawImage 实现 ===

RawImage::RawImage() = default;

RawImage::RawImage(int width, int height, PixelFormat format)
    : width_(width)
    , height_(height)
    , stride_(width * bytesPerPixelForFormat(format))
    , format_(format)
{
    int bpp = bytesPerPixelForFormat(format);
    size_t size = width * height * bpp;
    data_ = std::make_unique<uint8_t[]>(size);
    std::memset(data_.get(), 0, size);
}

RawImage::~RawImage() = default;

RawImage::RawImage(RawImage&& other) noexcept
    : width_(other.width_)
    , height_(other.height_)
    , stride_(other.stride_)
    , format_(other.format_)
    , data_(std::move(other.data_))
{
    other.width_ = 0;
    other.height_ = 0;
    other.stride_ = 0;
}

RawImage& RawImage::operator=(RawImage&& other) noexcept {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        stride_ = other.stride_;
        format_ = other.format_;
        data_ = std::move(other.data_);

        other.width_ = 0;
        other.height_ = 0;
        other.stride_ = 0;
    }
    return *this;
}

int RawImage::bytesPerPixel() const {
    return bytesPerPixelForFormat(format_);
}

bool RawImage::save(const std::string& filepath, int quality) const {
    // 简化实现：实际需要 stb_image 或其他库
    // 这里只做占位
    (void)filepath;
    (void)quality;
    return false;  // TODO: 实现
}

RawImage RawImage::convertTo(PixelFormat target_format) const {
    if (format_ == target_format || !data_) {
        // 不能返回 *this，创建一个副本
        RawImage copy(width_, height_, format_);
        std::memcpy(copy.data_.get(), data_.get(), width_ * height_ * bytesPerPixel());
        return copy;
    }

    RawImage result(width_, height_, target_format);

    // 简化：只实现 RGB888 -> RGBA8888
    if (format_ == PixelFormat::RGB888 && target_format == PixelFormat::RGBA8888) {
        const uint8_t* src = data_.get();
        uint8_t* dst = result.data_.get();

        for (int i = 0; i < width_ * height_; ++i) {
            dst[i * 4 + 0] = src[i * 3 + 0];  // R
            dst[i * 4 + 1] = src[i * 3 + 1];  // G
            dst[i * 4 + 2] = src[i * 3 + 2];  // B
            dst[i * 4 + 3] = 255;             // A
        }
    }

    return result;
}

RawImage RawImage::resize(int new_width, int new_height) const {
    if (!data_ || new_width <= 0 || new_height <= 0) {
        return RawImage();
    }

    RawImage result(new_width, new_height, format_);

    // 简化：最近邻插值
    float x_ratio = static_cast<float>(width_) / new_width;
    float y_ratio = static_cast<float>(height_) / new_height;
    int bpp = bytesPerPixel();

    const uint8_t* src = data_.get();
    uint8_t* dst = result.data_.get();

    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            int src_x = static_cast<int>(x * x_ratio);
            int src_y = static_cast<int>(y * y_ratio);

            if (src_x >= width_) src_x = width_ - 1;
            if (src_y >= height_) src_y = height_ - 1;

            int src_offset = (src_y * width_ + src_x) * bpp;
            int dst_offset = (y * new_width + x) * bpp;

            std::memcpy(dst + dst_offset, src + src_offset, bpp);
        }
    }

    return result;
}

void RawImage::swap(RawImage& other) noexcept {
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(stride_, other.stride_);
    std::swap(format_, other.format_);
    std::swap(data_, other.data_);
}

} // namespace PixRaw
