
//
// Created by igor on 10/27/16.
//

#include <cstdint>
#include <fstream>
#include <iostream>

namespace {
    template<typename T>
    inline void Write(T val, std::fstream& out) {
        out.write((char*)&val, sizeof(T));
    }
} // anonymous namespace


int gen_long_sum(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " [ROWS] [COLS] [FILENAME]" << std::endl;
        return 1;
    }
    uint32_t n = static_cast<uint32_t>(atoi(argv[1]));
    uint32_t m = static_cast<uint32_t>(atoi(argv[2]));
    std::fstream out(argv[3], std::fstream::out | std::fstream::binary);
    Write(n, out);
    for (uint32_t iter = 1; iter <= n; ++iter) {
        Write<uint8_t>(iter % 256, out);
    }
    Write(m, out);
    for (uint32_t iter = 1; iter <= m; ++iter) {
        Write<uint8_t>(iter % 256, out);
    }
    out.close();
}

int gen_graph(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " [ROWS] [COLS] [FILENAME]" << std::endl;
        return 1;
    }
    uint32_t n = static_cast<uint32_t>(atoi(argv[1]));
    uint32_t m = static_cast<uint32_t>(atoi(argv[2]));
    std::fstream out(argv[3], std::fstream::out | std::fstream::binary);
    Write(n, out);
    Write(m, out);

    for (uint32_t iter = 1; iter <= m; ++iter) {
        Write<uint32_t>(iter % 256, out);
        Write<uint32_t>((iter + 1) % 256, out);
    }
    out.close();
}

int gen_Ab(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " [ROWS] [COLS] [FILENAME]" << std::endl;
        return 1;
    }
    uint32_t n = static_cast<uint32_t>(atoi(argv[1]));
    uint32_t m = static_cast<uint32_t>(atoi(argv[2]));
    std::fstream out(argv[3], std::fstream::out | std::fstream::binary);
    Write(n, out);
    Write(m, out);

    for (uint32_t iter = 1; iter <= n*m; ++iter) {
        Write<uint8_t>(iter % 256, out);
    }
    for (uint32_t iter = 1; iter <= m; ++iter) {
        Write<uint8_t>(iter % 256, out);
    }
    out.close();
}

int main(int argc, char* argv[]) {
    gen_Ab(argc, argv);
    return 0;
}
