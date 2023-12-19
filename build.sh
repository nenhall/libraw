#!/bin/bash

# arch -arm64/-x86_64
script_directory=$(cd "$(dirname "$0")" && pwd)

# libraw 源文件目录
libraw_src_directory=${script_directory}/libraw

# 判定源码是否已经下载
if [ -z "$(ls -A "$libraw_src_directory")" ]; then
   echo "未找到 libraw 源码"
   echo  "开始下载 libraw 源码"
   sh init_libraw.sh
fi

# 编译输出目录
build_output_directory=${script_directory}/build-"$(arch)"

echo "脚本所在目录：$script_directory"
echo "Libraw 源文件路径：$libraw_src_directory"
echo "编译输出目录：$build_output_directory"

# 判定电脑是否安装 `autoreconf`
if command -v autoreconf &> /dev/null; then
    echo "autoreconf is installed."
else
    echo "autoreconf is not installed. Please install it before proceeding.(https://www.gnu.org/software/autoconf/)"
    # 可以添加安装 autoreconf 的命令，例如：
    case "$(uname -s)" in
        Darwin)
            echo "eg. macOS: brew install autoconf"
            ;;
        Linux)
            echo "eg. Linux: sudo apt-get install autoconf"
            ;;
        CYGWIN*|MINGW32*|MSYS*|MINGW*)
            echo "eg. Windows: Please install autoconf for Windows."
            ;;
        *)
            echo "Unknown OS: Please install autoconf"
            ;;
    esac
    exit 1
fi

cd "$libraw_src_directory" || exit
echo "开始编译......"

make clean

# 检查 configure.ac 和 Makefile.am 文件
autoreconf --install

# 指定安装路径
./configure --prefix="$build_output_directory"
make && make install

#arch -x86_64 ./configure CC="arch -x86_64 clang" CXX="arch -x86_64 clang++" --host=x86_64-apple-darwin --prefix="$build_output_directory"
#arch -x86_64 make && arch -x86_64 make install

