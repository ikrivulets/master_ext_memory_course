#include <iostream>
#include <fstream>
#include <stdlib.h>

class DisjointSet {
public:
    DisjointSet(uint32_t size = 0) {
        size_ = size;
        p = new uint32_t[size_];
//        rank = new uint32_t[size_];
        for (uint32_t index = 0; index < size_; index++) {
            p[index] = index;
//            rank[index] = 0;
        }
        num_of_comps_ = size_;
    }

    uint32_t Find(uint32_t x) {
        return ( x == p[x] ? x : p[x] = Find(p[x]) );
    }

    void Union (uint32_t x, uint32_t y) {
        if ((x = Find(x)) == (y = Find(y))) {
            return;
        }
        num_of_comps_--;
        if (rand() % 2) {
            p[x] = y;
        } else {
            p[y] = x;
        }
//        if ( rank[x] <  rank[y] ) {
//            p[x] = y;
//        } else {
//            p[y] = x;
//        }
//        if ( rank[x] == rank[y] ) {
//            ++rank[x];
//        }
    }

    uint32_t GetNumComps() {
        return num_of_comps_;
    }

private:
    uint32_t *p, size_, num_of_comps_;
};


class BufferedReader{
public:
    BufferedReader(std::string filename, uint32_t block_size,
                   uint32_t start_index, uint32_t end_index) {
        local_pos_ = 0;
        global_pos_ = start_index;
        block_size_ = block_size;
        end_index_ = end_index;
        filename_ = filename;
        input_.open(filename, std::ios::in | std::ios::binary);
        input_.seekg(start_index, std::ios_base::beg);
        cur_block_size_ = std::min(block_size, (end_index - start_index) / 4);
        block_ = new uint32_t[cur_block_size_];
        input_.read((char *) block_, cur_block_size_ * sizeof(uint32_t));
    }

    ~BufferedReader() {
        delete[] block_;
        input_.close();
    }

    bool getNext(std::pair<uint32_t, uint32_t> &next_edge) {
        if (global_pos_ >= end_index_) {
            return false;
        }
        if (local_pos_ < cur_block_size_) {
            next_edge.first = block_[local_pos_++];
            next_edge.second = block_[local_pos_++];
            return true;
        } else {
            global_pos_ += local_pos_*sizeof(uint32_t);
            local_pos_ = 0;
            if (global_pos_ < end_index_) {
                cur_block_size_ = std::min(block_size_, (end_index_ - global_pos_) / 4);
                delete[] block_;
                block_ = new uint32_t[cur_block_size_];
                input_.read((char *) block_, cur_block_size_ * sizeof(uint32_t));
                next_edge.first = block_[local_pos_++];
                next_edge.second = block_[local_pos_++];
                return true;
            } else {
                return false;
            }
        }
    }

private:
    std::ifstream input_;
    uint32_t *block_, local_pos_, cur_block_size_;
    uint32_t global_pos_, block_size_, end_index_;
    std::string filename_;
};

void CountConnectedComponents(std::string input_filename, std::string output_filename,
                              uint32_t block_size, const uint32_t& n, const uint32_t& m) {
    BufferedReader graph_reader(input_filename, block_size, 8, 8 + 8*m);
    DisjointSet components(n);
    std::pair<uint32_t, uint32_t> edge  =std::make_pair(0, 0);
    int count = 0;
    while (graph_reader.getNext(edge)) {
        if (count == 0) {
            components.Union(edge.first - 1, 0);
        } else {
            components.Union(edge.first - 1, edge.second - 1);
        }
        count++;
    }
    std::ofstream output(output_filename , std::ios::out | std::ios::binary);
    uint32_t result = components.GetNumComps();
    output.write((char *) &result, sizeof(result));
    output.close();
}


int main() {
    std::string input_filename = "input.bin";
    std::string output_filename = "output.bin";
    std::ifstream input(input_filename , std::ios::in | std::ios::binary);
    uint32_t n, m, block_size = 200;
    input.read((char *) &n, sizeof(n));
    input.read((char *) &m, sizeof(m));
    input.close();

    CountConnectedComponents(input_filename, output_filename, block_size, n, m);

    return 0;
}
