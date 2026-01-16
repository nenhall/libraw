#ifndef RAW_PROCESSOR_IMAGE_ADJUSTER_H
#define RAW_PROCESSOR_IMAGE_ADJUSTER_H

#include <RawImage.h>
#include <RawAdjustments.h>

namespace PixRaw {

/**
 * @brief 图像后处理器
 *
 * 在 RAW 解码后应用各种图像调整
 */
class ImageAdjuster {
public:
    /**
     * @brief 应用调整参数到图像
     * @param image 原始图像
     * @param adjustments 调整参数
     * @return 处理后的图像
     */
    static RawImage applyAdjustments(const RawImage& image, const RawAdjustments& adjustments);

private:
    // 各种调整的底层实现
    static void applyExposure(uint8_t* data, int width, int height, float exposure);
    static void applyContrast(uint8_t* data, int width, int height, float contrast);
    static void applySaturation(uint8_t* data, int width, int height, float saturation);
    static void applyTemperature(uint8_t* data, int width, int height, float temperature);

    // 辅助函数
    static float clamp(float value, float min, float max);
    static uint8_t toUint8(float value);
};

} // namespace PixRaw

#endif // RAW_PROCESSOR_IMAGE_ADJUSTER_H
