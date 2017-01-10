//
// Created by igor on 10/27/16.
//

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdlib.h>


namespace {
    template<typename T>
    inline void Write(T val, std::fstream& out) {
//        std::cout << "generated " << val << std::endl;
        out.write((char*)&val, sizeof(T));
    }
} // anonymous namespace

int main(int argc, char* argv[]) {
    srand (time(NULL));
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [N] [FILENAME]" << std::endl;
        return 1;
    }

    uint64_t numsToGenerate = static_cast<uint64_t>(atoi(argv[1]));
//    uint32_t cols = static_cast<uint32_t>(atoi(argv[2]));
//    uint32_t numsToGenerate = rows * cols;

    std::fstream out(argv[2], std::fstream::out | std::fstream::binary);
//    Write(rows, out);
    Write(numsToGenerate, out);
    for (uint64_t iter = 1; iter <= numsToGenerate; ++iter) {
        Write<uint64_t>(rand() % 50, out);
    }
    out.close();
    return 0;
}
