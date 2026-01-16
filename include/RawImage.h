#ifndef RAW_PROCESSOR_RAW_IMAGE_H
#define RAW_PROCESSOR_RAW_IMAGE_H

#include <memory>
#include <cstdint>
#include <string>

namespace PixRaw {

enum class PixelFormat {
    RGB888,     // 24-bit RGB
    RGBA8888,   // 32-bit RGBA
    RGB565      // 16-bit RGB565
};

class RawImage {
public:
    RawImage();
    RawImage(int width, int height, PixelFormat format);
    ~RawImage();

    // 禁止拷贝
    RawImage(const RawImage&) = delete;
    RawImage& operator=(const RawImage&) = delete;

    // 移动
    RawImage(RawImage&&) noexcept;
    RawImage& operator=(RawImage&&) noexcept;

    // 数据访问
    uint8_t* data() { return data_.get(); }
    const uint8_t* data() const { return data_.get(); }

    // 兼容性：void* 版本
    void* dataVoid() { return data_.get(); }
    const void* dataVoid() const { return data_.get(); }

    int width() const { return width_; }
    int height() const { return height_; }
    int stride() const { return stride_; }
    PixelFormat format() const { return format_; }
    int bytesPerPixel() const;

    // 保存为文件
    bool save(const std::string& filepath, int quality = 90) const;

    // 转换格式
    RawImage convertTo(PixelFormat target_format) const;

    // 缩放
    RawImage resize(int new_width, int new_height) const;

    // 有效检查
    bool isValid() const { return data_ != nullptr; }

    // 交换
    void swap(RawImage& other) noexcept;

private:
    int width_ = 0;
    int height_ = 0;
    int stride_ = 0;
    PixelFormat format_ = PixelFormat::RGB888;
    std::unique_ptr<uint8_t[]> data_;
};

} // namespace PixRaw

#endif // RAW_PROCESSOR_RAW_IMAGE_H
