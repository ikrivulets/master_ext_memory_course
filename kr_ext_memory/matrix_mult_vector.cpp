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
        input_.seekg(start_index, std::ios_base::beg);
        cur_block_size_ = std::min(block_size, (end_index - start_index));
        block_ = new uint8_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_);
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
        input_.seekg(global_pos_, std::ios_base::beg);
        block_ = new uint8_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_);
    }

    bool getNext(uint8_t& next_elem) {
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
                block_ = new uint8_t[cur_block_size_];
                input_.read((char *) block_, cur_block_size_);
                next_elem = block_[local_pos_++];
                return true;
            } else {
                return false;
            }
        }
    }

private:
    std::ifstream input_;
    uint8_t* block_;
    uint64_t local_pos_, cur_block_size_;
    uint64_t global_pos_, block_size_, end_index_;
    std::string filename_;
};


void Multiply(std::string input_filename, std::string output_filename,
              uint32_t& N, uint32_t& M, uint64_t& block_size) {
    uint64_t matrix_start_index = 8;
    uint64_t matrix_end_index = 8 + N*M;
    uint64_t vector_start_index = 8 + N*M;
    uint64_t vector_end_index = 8 + N*M + M;
    BufferedReader matrix_reader(input_filename, block_size, matrix_start_index, matrix_end_index);
    uint8_t mat_el = 0, vec_el = 0, res_el = 0;
    uint8_t *res_block = new uint8_t[block_size];
    uint64_t res_index = 0;

    std::ofstream output(output_filename , std::ios::out | std::ios::binary);
    for (int row_index = 0; row_index < N; row_index++) {
        res_el = 0;
        BufferedReader vector_reader(input_filename, block_size, vector_start_index, vector_end_index);
        for (int col_index = 0; col_index < M; col_index++) {
            matrix_reader.getNext(mat_el);
            vector_reader.getNext(vec_el);
            res_el += mat_el * vec_el;
        }
        res_block[res_index++] = res_el;
        if (res_index == block_size) {
            res_index = 0;
            output.write((char *) res_block, block_size * sizeof(uint8_t));
        }
    }
    if (res_index > 0) {
        output.write((char *) res_block, res_index * sizeof(uint8_t));
        delete[] res_block;
    }
    output.close();
}


int main() {
    std::string input_filename = "input.bin";
    std::string output_filename = "output.bin";
    std::ifstream input(input_filename , std::ios::in | std::ios::binary);
    uint32_t N, M;
    uint64_t block_size = 185000;
    input.read((char *) &N, sizeof(N));
    input.read((char *) &M, sizeof(M));
    input.close();
    Multiply(input_filename, output_filename, N, M, block_size);
    return 0;
}
