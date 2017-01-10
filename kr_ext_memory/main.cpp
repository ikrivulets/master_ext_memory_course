#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>


class BufferedReader{
public:
    BufferedReader(std::string filename, uint64_t block_size,
                   uint64_t start_index, uint64_t end_index) {
        local_pos_ = 0;
        global_pos_ = start_index;
        block_size_ = block_size;
        end_index_ = end_index;
        filename_ = filename;
        input_.open(filename, std::ios::in | std::ios::binary);
        input_.seekg(start_index * sizeof(uint64_t), std::ios_base::beg);
        cur_block_size_ = std::min(block_size, (end_index - start_index));
        block_ = new uint64_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_ * sizeof(uint64_t));
    }

    ~BufferedReader() {
        delete[] block_;
        input_.close();
    }

    BufferedReader(const BufferedReader& other) {
        local_pos_ = other.local_pos_;
        global_pos_ = other.global_pos_;
        block_size_ = other.block_size_;
        cur_block_size_ = other.cur_block_size_;
        end_index_ = other.end_index_;
        filename_ = other.filename_;
        input_.open(filename_, std::ios::in | std::ios::binary);
        input_.seekg((global_pos_) * sizeof(uint64_t), std::ios_base::beg);
        block_ = new uint64_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_ * sizeof(uint64_t));
    }

    bool getNext(uint64_t& next_elem) {
        if (global_pos_ >= end_index_) {
            return false;
        }
        if (local_pos_ < cur_block_size_) {
            next_elem = block_[local_pos_++];
            return true;
        } else {
            global_pos_ += local_pos_;
            local_pos_ = 0;
            if (global_pos_ < end_index_) {
                cur_block_size_ = std::min(block_size_, end_index_ - global_pos_);
                delete[] block_;
                block_ = new uint64_t[cur_block_size_];
                input_.read((char *) block_, cur_block_size_ * sizeof(uint64_t));
                next_elem = block_[local_pos_++];
                return true;
            } else {
                return false;
            }
        }
    }

private:
    std::ifstream input_;
    uint64_t* block_;
    uint64_t local_pos_, cur_block_size_;
    uint64_t global_pos_, block_size_, end_index_;
    std::string filename_;
};

uint64_t* ReadBlock(std::ifstream& input, const uint64_t& block_size, const uint64_t& offset) {
    uint64_t* block = new uint64_t[block_size];
    input.seekg(offset, std::ios_base::beg);
    input.read((char *) block, block_size * sizeof(uint64_t));
    return block;
}

void WriteBlock(std::ofstream& output, uint64_t* block, const uint64_t& block_size) {
    output.write((char *) block, block_size * sizeof(uint64_t));
}

void SortByBlocks(std::ifstream& input, std::ofstream& output, const uint64_t& block_size, const uint64_t& data_size) {
    input.read((char *) &data_size, sizeof(data_size));
    for (uint64_t index = 0; index < data_size/(float)block_size; ++index) {
        uint64_t cur_block_size;
        if (index < (data_size / block_size)) {
            cur_block_size = block_size;
        } else {
            cur_block_size = data_size - (data_size / block_size) * block_size;
        }
        uint64_t* block = ReadBlock(input, cur_block_size, 8 + index * block_size * sizeof(uint64_t));
        std::sort(block, block + cur_block_size);
        WriteBlock(output, block, cur_block_size);
        delete[] block;
    }
}

void AnotherMergeSorted(std::string filename1, const uint64_t& memory_size,
                        const uint64_t& block_size, const uint64_t& data_size) {
    std::ofstream output("output.bin", std::ios::out | std::ios::binary);
//    uint64_t num_parts = data_size / memory_size + 1;
    std::vector<BufferedReader> bf_readers;
    std::priority_queue<std::pair<uint64_t, uint64_t>,
            std::vector<std::pair<uint64_t, uint64_t >>,
            std::greater<std::pair<uint64_t, uint64_t >>> prior_queue;
    uint64_t* write_block = new uint64_t[block_size];
    uint64_t cur_write_pos = 0;

    // creating buffered readers for each part
    for (uint64_t cur_part = 0; cur_part < data_size/(float)memory_size; ++cur_part) {
        uint64_t end_index = std::min((cur_part + 1) * memory_size, data_size);
        bf_readers.push_back(BufferedReader(filename1, block_size, cur_part * memory_size, end_index));
        uint64_t next_el;
        if (bf_readers[cur_part].getNext(next_el)) {
            prior_queue.push(std::make_pair(next_el, cur_part));
        }
    }
    output.write((char *) &data_size, sizeof(data_size));
    while (!prior_queue.empty()) {
        std::pair<uint64_t, uint64_t> cur_pair = prior_queue.top();
        prior_queue.pop();
        write_block[cur_write_pos++] = cur_pair.first;
        if (cur_write_pos == block_size) {

            WriteBlock(output, write_block, block_size);
            cur_write_pos = 0;
            delete[] write_block;
            write_block = new uint64_t[block_size];
        }
        uint64_t next_elem;
        if (bf_readers[cur_pair.second].getNext(next_elem)) {
            prior_queue.push(std::make_pair(next_elem, cur_pair.second));
        }
    }
    WriteBlock(output, write_block, cur_write_pos);
    output.close();
    delete[] write_block;
}

void JustRewrite(std::string input_filename, std::string output_filename, uint64_t data_size) {
    std::ifstream input(input_filename, std::ios::in | std::ios::binary);
    std::ofstream output(output_filename, std::ios::out | std::ios::binary);
    output.write((char *) &data_size, sizeof(data_size));
    uint64_t* write_block = ReadBlock(input, data_size, 0);
    WriteBlock(output, write_block, data_size);
}

int main() {
    uint64_t  data_size;
    uint64_t  memory_size = 30000;
    uint64_t block_size = 50;
    std::string filename1 = "sorted_by_blocks.bin";
    std::ifstream input("input.bin", std::ios::in | std::ios::binary);
    std::ofstream output(filename1, std::ios::out | std::ios::binary);
    SortByBlocks(input, output, memory_size, data_size);
    input.close();
    output.close();
    if (memory_size < data_size) {
        AnotherMergeSorted(filename1, memory_size, block_size, data_size);
    } else {
        JustRewrite(filename1, "output.bin", data_size);
    }
    return 0;
}


void printArray(std::string input_filename) {
    std::ifstream input(input_filename, std::ios::in | std::ios::binary);
    uint32_t res_len;
    input.read((char *) &res_len, sizeof(res_len));
    uint8_t *data = new uint8_t[res_len];

    std::cout << "array: " << res_len <<  std::endl;
    for (uint32_t i = 0; i < res_len; ++i) {
        input.read((char *) data, sizeof(uint8_t));
        std::cout << (size_t)*data << " ";
        data++;
        std::cout << std::endl;
    }
    std::cout << std::endl << " array end"<< std::endl;
}