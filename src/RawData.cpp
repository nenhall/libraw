#include "RawData.h"
#include <cstring>

namespace PixRaw {

RawData::RawData() = default;

RawData::~RawData() = default;

RawData::RawData(RawData&& other) noexcept
    : data_(std::move(other.data_))
    , size_(other.size_)
{
    other.size_ = 0;
}

RawData& RawData::operator=(RawData&& other) noexcept {
    if (this != &other) {
        data_ = std::move(other.data_);
        size_ = other.size_;
        other.size_ = 0;
    }
    return *this;
}

RawData::RawData(const uint8_t* data, size_t size)
    : size_(size)
{
    if (data && size > 0) {
        data_ = std::make_unique<uint8_t[]>(size);
        std::memcpy(data_.get(), data, size);
    }
}

} // namespace PixRaw
