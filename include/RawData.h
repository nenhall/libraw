#ifndef RAW_PROCESSOR_RAW_DATA_H
#define RAW_PROCESSOR_RAW_DATA_H

#include <memory>
#include <cstdint>
#include <vector>

namespace PixRaw {

/**
 * @brief 原始字节数据容器
 * 用于存储 JPEG、PNG 等编码格式的图像数据
 */
class RawData {
public:
    RawData();
    ~RawData();

    // 禁止拷贝
    RawData(const RawData&) = delete;
    RawData& operator=(const RawData&) = delete;

    // 移动
    RawData(RawData&&) noexcept;
    RawData& operator=(RawData&&) noexcept;

    // 从指针构造
    RawData(const uint8_t* data, size_t size);

    // 访问数据
    const uint8_t* data() const { return data_.get(); }
    size_t size() const { return size_; }
    bool isValid() const { return data_ != nullptr && size_ > 0; }

private:
    std::unique_ptr<uint8_t[]> data_;
    size_t size_ = 0;
};

} // namespace PixRaw

#endif // RAW_PROCESSOR_RAW_DATA_H
