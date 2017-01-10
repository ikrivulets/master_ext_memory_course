#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


uint64_t* ReadBlock(std::ifstream& input, const uint64_t& block_size, const uint64_t& offset) {
    uint64_t* block = new uint64_t[block_size];
//    std::cout << "blocksize" << block_size << std::endl;
    input.seekg(offset, std::ios_base::beg);
    input.read((char *) block, block_size * sizeof(uint64_t));
    return block;
}

void WriteBlock(std::ofstream& output, uint64_t* block,
                const uint64_t& block_size) {
    output.write((char *) block, block_size * sizeof(uint64_t));
}

void SortByBlocks(std::ifstream& input, std::ofstream& output, const uint64_t& block_size, const uint64_t& data_size) {
    input.read((char *) &data_size, sizeof(data_size));
//    std::cout << "writing" << data_size << std::endl;

    for (uint64_t index = 0; index < data_size/(float)block_size; ++index) {
        if (index < (data_size / block_size)) {
            uint64_t* block = ReadBlock(input, block_size, 8 + index * block_size * sizeof(uint64_t));
//            std::cout << "index " << index << std::endl;
//            for (uint64_t i = 0; i < block_size; ++i) {
//                std::cout << block[i] << " ";
//            }
            std::sort(block, block + block_size);
//            std::cout << "after sort " << index << std::endl;
//            for (uint64_t i = 0; i < block_size; ++i) {
//                std::cout << block[i] << " ";
//            }
            WriteBlock(output, block, block_size);
            delete[] block;
//            std::cout << std::endl;
        } else {
            uint64_t bb = data_size - (data_size / block_size) * block_size;
            uint64_t* block = ReadBlock(input, bb, 8 + index * block_size * sizeof(uint64_t));
//            std::cout << "last index " << index << std::endl;
//            for (uint64_t i = 0; i < bb; ++i) {
//                std::cout << block[i] << " ";
//            }
            std::sort(block, block + bb);
//            for (uint64_t i = 0; i < bb; ++i) {
//                std::cout << block[i] << " ";
//            }
            WriteBlock(output, block, bb);
            delete[] block;
//            std::cout << std::endl;
        }
    }
}

bool MergeSorted(std::string filename1, std::string filename2, const uint64_t& block_size, const uint64_t& data_size) {
    uint64_t current_block_size = block_size;
    bool read_from_first = true;
    std::ifstream input;
    std::ofstream output;

    while (current_block_size < data_size) {
        if (read_from_first) {
            input.open(filename1, std::ios::in | std::ios::binary);
            output.open(filename2, std::ios::out | std::ios::binary);
        } else {
            input.open(filename2, std::ios::in | std::ios::binary);
            output.open(filename1, std::ios::out | std::ios::binary);
        }

        for (uint64_t num_of_pair = 0; num_of_pair < data_size/(float)current_block_size; num_of_pair = num_of_pair + 2) {

            uint64_t all_first_readed = 0, all_second_readed = 0;
            uint64_t cur_first_readed = 0, cur_second_readed = 0;
            uint64_t start_index1 = num_of_pair * current_block_size;
            uint64_t start_index2 = (num_of_pair + 1) * current_block_size;
            uint64_t end_index1 = (num_of_pair + 1) * current_block_size;
            uint64_t end_index2 = (num_of_pair + 2) * current_block_size;
            if (data_size < end_index2) {
                end_index2 = data_size;
            }
            uint64_t b_size1 = block_size;
            uint64_t b_size2 = std::min(block_size, (end_index2 - start_index2));
            uint64_t* block_first = ReadBlock(input, block_size, (all_first_readed + start_index1) * sizeof(uint64_t));
            uint64_t* block_second = ReadBlock(input, b_size2, (all_second_readed + start_index2) * sizeof(uint64_t));

            while (all_first_readed < current_block_size && all_second_readed < (end_index2 - start_index2)) {

                while (cur_first_readed < b_size1 && cur_second_readed < b_size2) {
                    if (block_first[cur_first_readed] < block_second[cur_second_readed]) {
                        output.write((char *) &block_first[cur_first_readed], sizeof(uint64_t));
                        cur_first_readed++;
                    } else {
                        output.write((char *) &block_second[cur_second_readed], sizeof(uint64_t));
                        cur_second_readed++;
                    }
                }

                if (cur_first_readed  == b_size1) {
                    all_first_readed += cur_first_readed;
                    cur_first_readed = 0;
                    if (block_size < (current_block_size - all_first_readed)) {
                        b_size1 = block_size;
                    } else {
                        b_size1 = (current_block_size - all_first_readed);
                    }
                    delete[] block_first;
                    block_first = ReadBlock(input, b_size1, (all_first_readed + start_index1) * sizeof(uint64_t));
                }
                if (cur_second_readed == b_size2) {
                    all_second_readed += cur_second_readed;
                    cur_second_readed = 0;
                    if (block_size < ((end_index2 - start_index2) - all_second_readed)) {
                        b_size2 = block_size;
                    } else {
                        b_size2 = ((end_index2 - start_index2) - all_second_readed);
                    }
                    delete[] block_second;
                    block_second = ReadBlock(input, b_size2, (all_second_readed + start_index2) * sizeof(uint64_t));
                }
            }

            if (all_first_readed == current_block_size) {
                while (all_second_readed < (end_index2 - start_index2)) {
                    while (cur_second_readed < b_size2) {
                        output.write((char *) &block_second[cur_second_readed], sizeof(uint64_t));
                        cur_second_readed++;
                    }
                    all_second_readed += cur_second_readed;
                    cur_second_readed = 0;
                    if (block_size < ((end_index2 - start_index2) - all_second_readed)) {
                        b_size2 = block_size;
                    } else {
                        b_size2 = ((end_index2 - start_index2) - all_second_readed);
                    }
                    delete[] block_second;
                    block_second = ReadBlock(input, b_size2, (all_second_readed + start_index2) * sizeof(uint64_t));
                }
            }

            if (all_second_readed == (end_index2 - start_index2)) {
                while (cur_first_readed < b_size1) {
                    output.write((char *) &block_first[cur_first_readed], sizeof(uint64_t));
                    cur_first_readed++;
                }
                all_first_readed += cur_first_readed;
                cur_first_readed = 0;
                if (block_size < (current_block_size - all_first_readed)) {
                    b_size1 = block_size;
                } else {
                    b_size1 = (current_block_size - all_first_readed);
                }
                delete[] block_first;
                block_first = ReadBlock(input, b_size1, (all_first_readed + start_index1) * sizeof(uint64_t));
            }
            delete[] block_first;
            delete[] block_second;
        }
        input.close();
        output.close();

        current_block_size *= 2;
        if (read_from_first) {
            read_from_first = false;
        } else {
            read_from_first = true;
        }
    }
    if (read_from_first) {
        input.open(filename1, std::ios::in | std::ios::binary);
    } else {
        input.open(filename2, std::ios::in | std::ios::binary);
    }
    output.open("output.bin", std::ios::out | std::ios::binary);
    output.write((char *) &data_size, sizeof(data_size));
    for (uint64_t index = 0; index < data_size/(float)block_size; ++index) {
        if (index < (data_size / block_size)) {
            uint64_t *block = ReadBlock(input, block_size, index * block_size * sizeof(uint64_t));
            WriteBlock(output, block, block_size);
            delete[] block;
        } else {
            uint64_t bb = data_size - (data_size / block_size) * block_size;
            uint64_t* block = ReadBlock(input, bb, index * block_size * sizeof(uint64_t));
            WriteBlock(output, block, bb);
            delete[] block;
        }
    }
    input.close();
    output.close();
    return read_from_first;
}

void readInput(std::ifstream& input, std::vector<uint64_t>& A) {
    uint64_t n;
    input.read((char *) &n, sizeof(n));
//    std::cout << "reading" << n << std::endl;
    for (uint64_t i = 0; i < n; i++) {
        uint64_t elem;
        input.read((char *) &elem, sizeof(elem));
        A.push_back(elem);
    }
}

void writeOutput(std::ofstream& output, std::vector<uint64_t>& data) {
    uint64_t size = data.size();
    output.write((char *) &size, sizeof(size));
    for (uint64_t i = 0; i < size; ++i) {
        output.write((char *) &data[i], sizeof(data[i]));
    }
}


int main() {
    uint64_t  data_size;
    uint64_t  memory_size = 3000;
    uint64_t block_size = 200;
    std::ifstream input("input.bin", std::ios::in | std::ios::binary);
    std::ofstream output("sorted_by_blocks.bin", std::ios::out | std::ios::binary);
    SortByBlocks(input, output, memory_size, data_size);
    input.close();
    output.close();

    std::string filename1 = "sorted_by_blocks.bin";
    std::string filename2 = "sorted_by_blocks2.bin";
    MergeSorted(filename1, filename2, block_size, data_size);
//    std::vector<uint64_t> A;
//    std::ifstream inpueet("output.bin", std::ios::in | std::ios::binary);
//    readInput(inpueet, A);
//    std::cout << "----result----" << data_size << std::endl;

//    for (auto e: A ) {
//        std::cout << e << " ";
//    }
//    std::cout << std::endl << "-------------" << std::endl;

    return 0;
}
