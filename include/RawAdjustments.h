#ifndef RAW_PROCESSOR_RAW_ADJUSTMENTS_H
#define RAW_PROCESSOR_RAW_ADJUSTMENTS_H

namespace PixRaw {

/**
 * @brief RAW 图像调整参数
 *
 * 用于在解码过程中应用各种图像调整
 */
struct RawAdjustments {
    // 基础调整
    float exposure = 0.0f;      // 曝光补偿 (-2.0 ~ 2.0 EV)
    float contrast = 0.0f;      // 对比度 (-50 ~ 50)
    float highlights = 0.0f;    // 高光 (-100 ~ 100)
    float shadows = 0.0f;       // 阴影 (-100 ~ 100)
    float saturation = 0.0f;    // 饱和度 (-100 ~ 100)
    float temperature = 0.0f;   // 色温 (-100 ~ 100, 负数=冷, 正数=暖)

    // 是否启用了任何调整
    bool hasAdjustments() const {
        return exposure != 0.0f ||
               contrast != 0.0f ||
               highlights != 0.0f ||
               shadows != 0.0f ||
               saturation != 0.0f ||
               temperature != 0.0f;
    }

    // 重置所有参数
    void reset() {
        exposure = 0.0f;
        contrast = 0.0f;
        highlights = 0.0f;
        shadows = 0.0f;
        saturation = 0.0f;
        temperature = 0.0f;
    }
};

} // namespace PixRaw

#endif // RAW_PROCESSOR_RAW_ADJUSTMENTS_H
