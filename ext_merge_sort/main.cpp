#include <iostream>
#include <fstream>

void testMerge(std::ifstream& input) {
    std::cout << "lets check reading by blocks" << std::endl;
    std::cout << sizeof(uint64_t);
    uint64_t size;
    input.read((char *) &size, sizeof(size));
    uint64_t block
    for (int i = 0; i < size; ++i) {
        ui
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::ifstream input1("input.bin" , std::ios::in | std::ios::binary);
    testMerge(input1);
    return 0;
}


void printArray(std::ifstream& input, ) {
    uint32_t rows, cols;
    input.read((char *) &rows, sizeof(rows));
    input.read((char *) &cols, sizeof(cols));
    uint8_t *data = new uint8_t[rows * cols];
    std::cout << "array: " << std::endl;
    for (uint32_t i = 0; i < rows; ++i) {
        for (uint32_t j = 0; j < cols; ++j) {
            input.read((char *) data, sizeof(uint8_t));
            std::cout << (size_t)*data << " ";
            data++;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << " array end"<< std::endl;
}



uint8_t* readBlock(std::ifstream& input, const uint32_t& rows, const uint32_t& cols,
                   const uint32_t& block_rows, const uint32_t& block_cols,
                   const uint32_t& offset_rows, const uint32_t& offset_cols) {
    uint8_t* block = new uint8_t[block_rows * block_cols];
    for (uint32_t row_index = 0; row_index < block_rows; ++row_index) {
        uint32_t seek_index = 8 + (offset_rows + row_index) * block_cols + offset_cols;
        input.seekg(seek_index);
        input.read((char *)(block + row_index * block_cols), block_cols);
    }
    return block;
}

void writeBlock(std::ofstream& output, const uint8_t* block,
                const uint32_t& rows, const uint32_t& cols,
                const uint32_t& block_rows, const uint32_t& block_cols,
                const uint32_t& offset_rows, const uint32_t& offset_cols) {
    for (uint32_t row_index = 0; row_index < block_rows; ++row_index) {
        uint32_t seek_index = 8 + (offset_rows + row_index) * block_cols + offset_cols;
        output.seekp(seek_index);
        output.write((char *) (block + row_index * block_cols), block_cols);
    }
}

uint8_t* transposeBlock(uint8_t* initial_data, uint32_t block_rows, uint32_t block_cols) {
    uint8_t* transposed_data = new uint8_t[block_cols * block_rows];
    for (uint32_t row_index = 0; row_index < block_rows; ++row_index) {
        for (uint32_t col_index = 0; col_index < block_cols; ++col_index) {
            transposed_data[col_index * block_rows + row_index] =
                    initial_data[row_index * block_cols + col_index];
        }
    }
    return transposed_data;
}


void externalTranspose() {
    uint32_t num_of_rows, num_of_cols;
    uint32_t block_cols = 300, block_rows = 300;
    uint32_t block_size = block_cols * block_rows;
    std::ifstream input("input.bin" , std::ios::in | std::ios::binary);
    std::ofstream output("output.bin", std::ios::out | std::ios::binary);

    input.read((char *) &num_of_rows, sizeof(num_of_rows));
    input.read((char *) &num_of_cols, sizeof(num_of_cols));
    output.write((char *) &num_of_cols, sizeof(num_of_cols));
    output.write((char *) &num_of_rows, sizeof(num_of_rows));


    if (num_of_cols * num_of_rows < block_cols * block_rows) {
        block_cols = num_of_cols;
        block_rows = num_of_rows;
    } else {
        if (num_of_rows < block_rows) {
            block_rows = num_of_rows;
            block_cols = block_size / block_rows;
        } else if (num_of_cols < block_cols) {
            block_cols = num_of_cols;
            block_rows = block_size / block_cols;
        }
    }

    for (uint32_t i = 0; i < num_of_rows / (double) block_rows; ++i) {
        uint32_t offset_rows = i * block_rows;
        for (uint32_t j = 0; j < num_of_cols / (double) block_cols; ++j) {
            uint32_t offset_cols = j * block_cols;
            uint32_t counted_block_rows, counted_block_cols;
            if ((offset_rows + block_rows) < num_of_rows) {
                counted_block_rows = block_rows;
            } else {
                counted_block_rows = num_of_rows - offset_rows;
            }

            if ((offset_cols + block_cols) < num_of_cols) {
                counted_block_cols = block_cols;
            } else {
                counted_block_cols = num_of_cols - offset_cols;
            }

            uint8_t* block = readBlock(input, num_of_rows, num_of_cols,
                                       counted_block_rows, counted_block_cols,
                                       offset_rows, offset_cols);

            uint8_t* transposed_block = transposeBlock(block, counted_block_rows, counted_block_cols);
            delete block;

            writeBlock(output, transposed_block, num_of_cols, num_of_rows,
                       counted_block_cols, counted_block_rows,
                       offset_cols, offset_rows);

            delete transposed_block;
        }
    }

    input.close();
    output.close();
}

