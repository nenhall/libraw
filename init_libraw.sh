#!/bin/sh

# 当前脚本的目录
script_dir=$(cd "$(dirname "$0")" && pwd)
echo "当前脚本的目录: $script_dir"

libraw_src_dir="$script_dir/libraw"

# 判断目录是否存在
if [ -d "$libraw_src_dir" ]; then
    # 目录存在
    # 判断目录是否为空
    if [ -z "$(ls -A "$libraw_src_dir")" ]; then
        # 目录为空
        rm -rf "$libraw_src_dir"
    else
        # shellcheck disable=SC2039
        read -r -p "libraw 源码已存在，是否重新克隆? [Y/n] " input
        case $input in
        [yY][eE][sS] | [yY])
            rm -rf "$libraw_src_dir"
            ;;
        [nN][oO] | [nN])
            echo "不重新克隆 libraw"
            exit 0
            ;;
        *)
            echo "无效的输入，请输入 Y 或 n"
            exit 1
            ;;
        esac
    fi
fi

echo "开始克隆子模块"
git submodule update --init --recursive

echo "克隆完成，切换到 0.21.1 分支"
cd libraw || exit
git checkout 0.21.1
cd ../
