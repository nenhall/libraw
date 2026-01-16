# PixRaw

一个轻量级的 RAW 图像处理 C++ 库，基于 LibRaw 构建，提供简洁的 API 用于读取、解码和处理相机 RAW 格式图像。

## 特性

- **多格式支持**: 支持 500+ 种相机的 RAW 格式（基于 LibRaw）
- **灵活的解码选项**: 提供快速预览、中等预览和全尺寸解码
- **图像调整**: 支持曝光、对比度、高光、阴影、饱和度和色温调整
- **元数据提取**: 读取 EXIF 信息和相机参数
- **高性能**: 启用 OpenMP 和 RawSpeed 加速
- **跨平台**: 支持 Windows、Linux 和 macOS
- **现代 C++**: 使用 C++17 标准，采用 RAII 和移动语义

## 依赖

- **LibRaw** (0.22.0): 核心RAW 解码库（通过 CMake FetchContent 自动下载）
- **CMake**: >= 3.16
- **C++ 编译器**: 支持 C++17 标准（MSVC、GCC、Clang）
- **OpenMP**: 用于并行处理（可选）

## 编译

### 基本编译

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目（LibRaw 将自动下载）
cmake ..

# 编译
cmake --build .
```

### 编译选项

```bash
# 启用 C API 构建
cmake .. -DPIX_RAW_BUILD_C_API=ON

# 启用安装目标
cmake .. -DPIX_RAW_INSTALL=ON
```

### 安装

```bash
cmake --build . --target install
```

## 使用示例

### 基本用法

```cpp
#include <PixRaw.h>

using namespace PixRaw;

int main() {
    PixRaw processor;

    // 打开 RAW 文件
    if (!processor.open("photo.CR3")) {
        printf("Error: %s\n", processor.getLastError().c_str());
        return 1;
    }

    // 获取元数据
    RawMetadata meta = processor.getMetadata();
    printf("Camera: %s %s\n", meta.camera_make.c_str(), meta.camera_model.c_str());
    printf("Size: %d x %d\n", meta.image_width, meta.image_height);
    printf("ISO: %.0f\n", meta.iso);

    // 解码预览图
    RawImage preview = processor.decodePreview(1920, 1080);
    if (preview.isValid()) {
        preview.save("preview.jpg");
    }

    return 0;
}
```

### 设置图像调整

```cpp
#include <PixRaw.h>
#include <RawAdjustments.h>

using namespace PixRaw;

int main() {
    PixRaw processor;
    processor.open("photo.ARW");

    // 设置调整参数
    RawAdjustments adjustments;
    adjustments.exposure = 0.5f;      // +0.5 EV 曝光
    adjustments.contrast = 10.0f;     // 增加对比度
    adjustments.shadows = 20.0f;      // 提亮阴影
    adjustments.temperature = -10.0f; // 稍微冷色调

    processor.setAdjustments(adjustments);

    // 解码时应用调整
    RawImage image = processor.decodeFull();
    image.save("adjusted.jpg");

    return 0;
}
```

### 快速预览

```cpp
// 超快速预览（用于立即显示，约 320x240）
RawImage quick = processor.decodeQuickPreview();

// 中等预览（平衡质量和速度，约 1280x720）
RawImage medium = processor.decodeMediumPreview();

// 获取缩略图
RawImage thumb = processor.getThumbnail();
```

## API 参考

### PixRaw 类

主要的 RAW 处理类。

| 方法 | 说明 |
|------|------|
| `open(string/wstring)` | 打开 RAW 文件 |
| `getMetadata()` | 获取图像元数据 |
| `decodePreview(max_w, max_h)` | 解码预览图（自适应大小） |
| `decodeQuickPreview()` | 解码超快速预览（约 320x240） |
| `decodeMediumPreview()` | 解码中等预览（约 1280x720） |
| `decodeFull()` | 解码全尺寸图像 |
| `getThumbnail()` | 获取缩略图（RGB） |
| `getThumbnailData()` | 获取缩略图原始 JPEG 数据 |
| `setAdjustments()` | 设置图像调整参数 |
| `getAdjustments()` | 获取当前调整参数 |
| `getLastError()` | 获取最后的错误信息 |
| `isOpen()` | 检查文件是否已打开 |
| `close()` | 关闭当前文件 |

### RawImage 类

表示解码后的图像数据。

| 方法 | 说明 |
|------|------|
| `data()` | 获取图像数据指针 |
| `width()`, `height()` | 获取图像尺寸 |
| `stride()` | 获取行跨度 |
| `format()` | 获取像素格式 |
| `bytesPerPixel()` | 获取每像素字节数 |
| `save(path, quality)` | 保存为文件（支持 JPG/PNG） |
| `convertTo(format)` | 转换像素格式 |
| `resize(w, h)` | 调整图像大小 |
| `isValid()` | 检查图像是否有效 |

### RawMetadata 结构

存储图像的元数据信息。

| 字段 | 类型 | 说明 |
|------|------|------|
| `camera_make` | string | 相机制造商 |
| `camera_model` | string | 相机型号 |
| `software` | string | 软件版本 |
| `image_width/height` | int | 图像尺寸 |
| `raw_width/height` | int | RAW 尺寸 |
| `iso` | double | ISO 感光度 |
| `shutter_speed` | double | 快门速度（秒） |
| `aperture` | double | 光圈值 |
| `focal_length` | double | 焦距（mm） |
| `timestamp` | int64_t | Unix 时间戳 |
| `wb_red/green/blue` | float | 白平衡系数 |
| `lens_model` | string | 镜头型号 |
| `orientation` | int | EXIF 方向 |

### RawAdjustments 结构

图像调整参数。

| 字段 | 范围 | 说明 |
|------|------|------|
| `exposure` | -2.0 ~ 2.0 EV | 曝光补偿 |
| `contrast` | -50 ~ 50 | 对比度 |
| `highlights` | -100 ~ 100 | 高光 |
| `shadows` | -100 ~ 100 | 阴影 |
| `saturation` | -100 ~ 100 | 饱和度 |
| `temperature` | -100 ~ 100 | 色温（负=冷，正=暖） |

## 项目结构

```
PixRaw/
├── include/           # 公共头文件
├── src/              # 源文件实现
├── third_party/      # 第三方依赖（LibRaw 自动下载）
└── CMakeLists.txt    # 构建配置
```

## 支持的 RAW 格式

基于 LibRaw，支持主流相机的 RAW 格式：

- **Canon**: CRW, CR2, CR3
- **Nikon**: NEF, NRW
- **Sony**: ARW, SRF, SR2
- **Adobe**: DNG
- **Fujifilm**: RAF
- **Olympus**: ORF
- **Panasonic**: RAW, RW2
- **Pentax**: PEF, RAW
- **Leica**: DNG, RWL
- 以及 500+ 种其他格式

## 性能优化

- **OpenMP**: 自动并行化图像处理
- **RawSpeed**: 使用优化的解码器
- **移动语义**: 避免不必要的拷贝
- **智能指针**: 自动内存管理

## 许可证

本项目使用 MIT 许可证。

依赖的 LibRaw 库使用以下许可证：
- CDDL-1.0 或 LGPL-2.1
- 详见 [third_party/libraw/src/LICENSE.*](third_party/libraw/src/)

## 贡献

欢迎提交 Issue 和 Pull Request！

## 更新日志

### 1.0.0
- 初始版本
- 支持 RAW 文件读取和解码
- 支持图像调整（曝光、对比度、高光、阴影、饱和度、色温）
- 支持多种分辨率预览
- 元数据提取

## 相关链接

- [LibRaw 官方网站](https://www.libraw.org/)
- [LibRaw GitHub](https://github.com/LibRaw/LibRaw)
- [支持的相机列表](https://www.libraw.org/docs/Sources-Cameras.html)
