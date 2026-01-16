#include "ImageAdjuster.h"
#include <cmath>
#include <algorithm>

namespace PixRaw {

RawImage ImageAdjuster::applyAdjustments(const RawImage& image, const RawAdjustments& adjustments) {
    if (!image.isValid()) {
        return RawImage();  // 返回空图像
    }

    if (!adjustments.hasAdjustments()) {
        // 如果没有调整，需要创建一个副本（因为禁止拷贝）
        RawImage result(image.width(), image.height(), image.format());
        int dataSize = image.width() * image.height() * image.bytesPerPixel();
        std::memcpy(result.data(), image.data(), dataSize);
        return result;
    }

    // 创建图像副本
    RawImage result(image.width(), image.height(), image.format());
    int dataSize = image.width() * image.height() * image.bytesPerPixel();
    std::memcpy(result.data(), image.data(), dataSize);

    uint8_t* data = result.data();
    int width = result.width();
    int height = result.height();

    // 按顺序应用调整
    // 1. 曝光
    if (adjustments.exposure != 0.0f) {
        applyExposure(data, width, height, adjustments.exposure);
    }

    // 2. 对比度
    if (adjustments.contrast != 0.0f) {
        applyContrast(data, width, height, adjustments.contrast);
    }

    // 3. 饱和度
    if (adjustments.saturation != 0.0f) {
        applySaturation(data, width, height, adjustments.saturation);
    }

    // 4. 色温
    if (adjustments.temperature != 0.0f) {
        applyTemperature(data, width, height, adjustments.temperature);
    }

    return result;
}

void ImageAdjuster::applyExposure(uint8_t* data, int width, int height, float exposure) {
    // 曝光调整：使用对数刻度
    // exposure = 0.0 表示不变
    // exposure > 0.0 增加亮度
    // exposure < 0.0 减少亮度
    float factor = std::pow(2.0f, exposure);

    int pixelCount = width * height * 3;  // RGB888
    for (int i = 0; i < pixelCount; ++i) {
        float value = data[i] * factor;
        data[i] = toUint8(value);
    }
}

void ImageAdjuster::applyContrast(uint8_t* data, int width, int height, float contrast) {
    // 对比度调整
    // contrast = 0 表示不变
    // range: -50 to 50
    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));

    int pixelCount = width * height * 3;
    for (int i = 0; i < pixelCount; ++i) {
        float value = factor * (data[i] - 128.0f) + 128.0f;
        data[i] = toUint8(value);
    }
}

void ImageAdjuster::applySaturation(uint8_t* data, int width, int height, float saturation) {
    // 饱和度调整
    // saturation = 0 表示不变
    // range: -100 to 100
    float saturationFactor = 1.0f + (saturation / 100.0f);

    int pixelCount = width * height;
    for (int i = 0; i < pixelCount; ++i) {
        float r = data[i * 3];
        float g = data[i * 3 + 1];
        float b = data[i * 3 + 2];

        // 转换到 HSL，调整饱和度
        float max = std::max({r, g, b});
        float min = std::min({r, g, b});
        float gray = 0.299f * r + 0.587f * g + 0.114f * b;

        // 应用饱和度
        r = gray + (r - gray) * saturationFactor;
        g = gray + (g - gray) * saturationFactor;
        b = gray + (b - gray) * saturationFactor;

        data[i * 3] = toUint8(r);
        data[i * 3 + 1] = toUint8(g);
        data[i * 3 + 2] = toUint8(b);
    }
}

void ImageAdjuster::applyTemperature(uint8_t* data, int width, int height, float temperature) {
    // 色温调整
    // temperature = 0 表示不变
    // temperature < 0 偏冷（增加蓝色）
    // temperature > 0 偏暖（增加黄色/红色）
    float factor = temperature / 100.0f;

    int pixelCount = width * height;
    for (int i = 0; i < pixelCount; ++i) {
        float r = data[i * 3];
        float g = data[i * 3 + 1];
        float b = data[i * 3 + 2];

        // 应用色温偏移
        if (temperature > 0) {
            // 暖色调：增加红色，减少蓝色
            r = r + (255.0f - r) * factor * 0.5f;
            b = b * (1.0f - factor * 0.3f);
        } else {
            // 冷色调：减少红色，增加蓝色
            r = r * (1.0f + factor * 0.3f);
            b = b + (255.0f - b) * (-factor) * 0.5f;
        }

        data[i * 3] = toUint8(r);
        data[i * 3 + 1] = toUint8(g);
        data[i * 3 + 2] = toUint8(b);
    }
}

float ImageAdjuster::clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

uint8_t ImageAdjuster::toUint8(float value) {
    return static_cast<uint8_t>(clamp(value, 0.0f, 255.0f));
}

} // namespace PixRaw
