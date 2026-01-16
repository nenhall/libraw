#ifndef PIX_RAW_PIX_RAW_H
#define PIX_RAW_PIX_RAW_H

#include <RawAdjustments.h>
#include <RawData.h>
#include <RawImage.h>
#include <RawMetadata.h>
#include <memory>
#include <string>

namespace PixRaw {

class PixRaw {
public:
  PixRaw();
  ~PixRaw();

  // 禁止拷贝
  PixRaw(const PixRaw &) = delete;
  PixRaw &operator=(const PixRaw &) = delete;

  // 移动语义
  PixRaw(PixRaw &&) noexcept;
  PixRaw &operator=(PixRaw &&) noexcept;

  /**
   * 打开 RAW 文件
   * @param filepath 文件路径（UTF-8）
   * @return 成功返回 true
   */
  bool open(const std::string &filepath);

  /**
   * 打开 RAW 文件（宽字符版本，Windows）
   */
  bool open(const std::wstring &filepath);

  /**
   * 获取元数据
   */
  RawMetadata getMetadata() const;

  /**
   * 解码为预览图（自动调整大小）
   * @param max_width 最大宽度（0 表示自适应）
   * @param max_height 最大高度（0 表示自适应）
   */
  RawImage decodePreview(int max_width = 1920, int max_height = 1080);

  /**
   * 超快速预览（用于立即显示）
   * @return 低分辨率预览图（约 320x240），非常快
   */
  RawImage decodeQuickPreview();

  /**
   * 中等预览（平衡质量和速度）
   * @return 中等分辨率预览图（约 1280x720）
   */
  RawImage decodeMediumPreview();

  /**
   * 解码全尺寸图像
   */
  RawImage decodeFull();

  /**
   * 获取缩略图（解码后的 RGB 图像）
   */
  RawImage getThumbnail();

  /**
   * 获取缩略图原始数据（JPEG 格式）
   * @return JPEG 数据，如果失败或不是 JPEG 格式返回空数据
   */
  RawData getThumbnailData();

  /**
   * 获取最后错误信息
   */
  std::string getLastError() const;

  /**
   * 检查文件是否已打开
   */
  bool isOpen() const;

  /**
   * 关闭当前文件
   */
  void close();

  /**
   * @brief 设置图像调整参数
   * @param adjustments 调整参数
   */
  void setAdjustments(const RawAdjustments &adjustments);

  /**
   * @brief 获取当前的调整参数
   */
  RawAdjustments getAdjustments() const;

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace PixRaw

#endif // PIX_RAW_PIX_RAW_H
