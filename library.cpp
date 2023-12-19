#include "library.h"
#include <iostream>
#include "libraw/libraw/libraw.h"

void readRawFile(const std::string& file) {
    LibRaw rawProcessor;
    rawProcessor.open_file(file.c_str());
    rawProcessor.unpack();
    rawProcessor.dcraw_process();
    auto imaget = rawProcessor.dcraw_make_mem_image();
    std::cout << imaget->data_size << std::endl;
    int input;
    std::cin >> input;
}

int main(int argc, char **argv) {
    std::cout << "argc: " << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "argv[" << i << "]：" << argv[i] << std::endl;
    }

    std::cout << "Hello, World!" << std::endl;
    std::string file;
    if (argc < 2) {
        file = "/Users/meitu/Pictures/Raw原图/_DSC0033.ARW";
        std::cout << "Please input raw file path" << std::endl;
    } else {
        file = argv[1];
        std::cout << "Input file path: " << argv[1] << std::endl;
    }
    readRawFile(file);

    int input;
    std::cin >> input;
    return 0;
}
