#include <iostream>
#include <fstream>


class BufferedReader{
public:
    BufferedReader(std::string filename, uint32_t block_size,
                   uint32_t start_index, uint32_t end_index) {
        global_pos_ = end_index;
        block_size_ = block_size;
        end_index_ = start_index;
        filename_ = filename;
        input_.open(filename, std::ios::in | std::ios::binary);
        cur_block_size_ = std::min(block_size, (end_index - start_index));
        input_.seekg(end_index - cur_block_size_, std::ios_base::beg);
        block_ = new uint8_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_);
        local_pos_ = cur_block_size_ - 1;
    }

    ~BufferedReader() {
        delete[] block_;
        input_.close();
    }

    bool getNext(uint8_t& next_elem) {
        if (global_pos_ < end_index_) {
            return false;
        }
        if (local_pos_ >= 0) {
            next_elem = block_[local_pos_--];
            return true;
        } else {
            global_pos_ -= cur_block_size_;
            if (global_pos_ > end_index_) {
                cur_block_size_ = std::min(block_size_, global_pos_ - end_index_);
                local_pos_ = cur_block_size_ - 1;
                delete[] block_;
                block_ = new uint8_t[cur_block_size_];
                input_.seekg(global_pos_ - cur_block_size_);
                input_.read((char *) block_, cur_block_size_);
                next_elem = block_[local_pos_--];
                return true;
            } else {
                return false;
            }
        }
    }

private:
    std::ifstream input_;
    uint8_t* block_;
    int local_pos_, cur_block_size_;
    int global_pos_, block_size_, end_index_;
    std::string filename_;
};

void CountSum(std::string input_filename, std::string output_filename,
               uint32_t block_size, uint32_t& A_len, const uint32_t& B_len) {

    std::ifstream input(input_filename, std::ios::in | std::ios::binary);
    std::ofstream output("tmp.bin", std::ios::out | std::ios::binary);
    input.read((char *) &A_len, sizeof(A_len));
    input.seekg(4 + A_len);
    input.read((char *) &B_len, sizeof(B_len));
    input.close();

    uint32_t start_index_first = 4;
    uint32_t end_index_first = 4 + A_len;
    uint32_t start_index_second = 8 + A_len;
    uint32_t end_index_second = 8 + A_len + B_len;
    BufferedReader number_A(input_filename, block_size,
                                       start_index_first, end_index_first);
    BufferedReader number_B(input_filename, block_size,
                                       start_index_second, end_index_second);

    uint8_t digit_A, digit_B, rest = 0;
    bool A_has_more = true, B_has_more = true;
    number_A.getNext(digit_A);
    number_B.getNext(digit_B);
    uint32_t C_len = 0;
    uint8_t *result_block = new uint8_t[block_size];
    uint32_t res_pos = 0;
    while (A_has_more || B_has_more) {
        uint32_t digit_C = digit_A + digit_B + rest;

        if (digit_C > 9) {
            rest = 1;
            digit_C -= 10;
        } else {
            rest = 0;
        }
        C_len++;
        result_block[res_pos++] = digit_C;
        digit_A = digit_B = 0;
        A_has_more = number_A.getNext(digit_A);
        B_has_more = number_B.getNext(digit_B);
        if (res_pos == block_size) {
            output.write((char *) result_block, block_size * sizeof(uint8_t));
            res_pos = 0;
        }

    }

    if (res_pos > 0) {
        output.write((char *) result_block, res_pos * sizeof(uint8_t));
        res_pos = 0;
    }
    if (rest > 0) {
        output.write((char *) &rest, sizeof(rest));
        C_len++;
    }
    output.close();

    BufferedReader reverse_buffer("tmp.bin", block_size, 0, C_len);
    uint8_t elem;

    output.open(output_filename, std::ios::out | std::ios::binary);
    output.write((char *) &C_len, sizeof(C_len));
    for (int i = 0; i < C_len; i++) {
        reverse_buffer.getNext(elem);
        result_block[res_pos++] = elem;
        if (res_pos == block_size) {
            output.write((char *) result_block, block_size * sizeof(uint8_t));
            res_pos = 0;
        }
    }
    if (res_pos > 0) {
        output.write((char *) result_block, res_pos * sizeof(uint8_t));
        delete[] result_block;
    }
    output.close();
}

int main() {
    uint32_t block_size = 20000;
    uint32_t A_len, B_len;
    std::string input_filename = "input.bin";
    std::string output_filename = "output.bin";
    CountSum(input_filename, output_filename, block_size, A_len, B_len);
    return 0;
}
