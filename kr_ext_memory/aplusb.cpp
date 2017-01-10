#include <iostream>
#include <fstream>



int main() {
    std::ifstream input("input.bin" , std::ios::in | std::ios::binary);
    uint32_t number_A, number_B, result;
    input.read((char *) &number_A, sizeof(number_A));
    input.read((char *) &number_B, sizeof(number_B));
    input.close();

    result = number_A + number_B;

    std::ofstream output("output.bin", std::ios::out | std::ios::binary);
    output.write((char *) &result, sizeof(result));
    output.close();
    return 0;
}
