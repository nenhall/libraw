#ifndef RAW_PROCESSOR_RAW_METADATA_H
#define RAW_PROCESSOR_RAW_METADATA_H

#include <string>
#include <cstdint>

namespace PixRaw {

struct RawMetadata {
    // 相机信息
    std::string camera_make;
    std::string camera_model;
    std::string software;

    // 图像尺寸
    int image_width = 0;
    int image_height = 0;
    int raw_width = 0;
    int raw_height = 0;

    // 拍摄参数
    double iso = 0.0;
    double shutter_speed = 0.0;  // 秒
    double aperture = 0.0;        // f-value
    double focal_length = 0.0;    // mm

    // 时间戳
    int64_t timestamp = 0;        // Unix 时间戳

    // 白平衡
    float wb_red = 1.0f;
    float wb_green = 1.0f;
    float wb_blue = 1.0f;

    // 镜头信息
    std::string lens_model;

    // 其他
    int orientation = 1;          // EXIF 方向
    bool is_raw = true;

    // 清空所有字段
    void clear() {
        camera_make.clear();
        camera_model.clear();
        software.clear();
        image_width = 0;
        image_height = 0;
        raw_width = 0;
        raw_height = 0;
        iso = 0.0;
        shutter_speed = 0.0;
        aperture = 0.0;
        focal_length = 0.0;
        timestamp = 0;
        wb_red = 1.0f;
        wb_green = 1.0f;
        wb_blue = 1.0f;
        lens_model.clear();
        orientation = 1;
        is_raw = true;
    }
};

} // namespace PixRaw

#endif // RAW_PROCESSOR_RAW_METADATA_H
