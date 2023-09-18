#include "huffman.hpp"

#include <set>
#include <vector>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <unordered_map>

using namespace cip;

struct NodeComparator {
    bool operator () (const Node *a, const Node *b) const {
        return *a < *b;
    }
};

void Node::supplyCode(std::string prefix) {
    this->code = prefix;
    
    if (this->left)
        this->left->supplyCode(prefix + '0');
    if (this->right)
        this->right->supplyCode(prefix + '1');
}

std::tuple<int, int, int> Archiver::encode(std::string filename, std::string out) const {
    int input_size = 0, header_size = 0, content_size = 0;
    
    if (filename.empty() || out.empty()) {
        throw std::runtime_error("Filename can not be empty");
    }
    
    std::ifstream file;
    file.exceptions(std::ifstream::badbit); // throws on file exeptions
    file.open(filename);
    
    if (!file.good()) {
        throw std::runtime_error("Could not open the file: " + filename);
    }
    
    std::unordered_map<char, Node*> nodes;
    
    char byte;
    while (file.get(byte)) {
        if (nodes.find(byte) == nodes.end()) {
            nodes[byte] = new Node(byte);
        }
        nodes[byte]->frequency++;
        input_size++;
    }
    
    assert(nodes.size() < UINT32_MAX);
    uint32_t count = (uint32_t)nodes.size();
    
    std::multiset<Node*, NodeComparator> sorted;
    
    for (auto &it : nodes) {
        sorted.insert(it.second);
    }
    
    while (sorted.size() > 1) {
        auto it = sorted.begin();
        Node *node1 = *it, *node2 = *std::next(it);
        sorted.erase(it, std::next(it, 2));
        sorted.insert(new Node('\0', node1->frequency + node2->frequency, node1, node2));
    }
    
    // calculate the number of bits that should be written
    uint32_t total = 0;
    
    Node* head = nullptr;
    if (!sorted.empty()) {
        // not an empty file
        assert(sorted.size() == 1);
        head = *sorted.begin();
        head->supplyCode("");
        
        for (auto &pair: nodes) {
            total += pair.second->code.length() * pair.second->frequency;
        }
    }
    
    std::ofstream output;
    output.exceptions(std::ofstream::badbit); // throws on file exeptions
    output.open(out, std::ios::out | std::ios::binary);
    
    if (!output.good()) {
        file.close();
        if (head) delete head;
        throw std::runtime_error("Could not open the file: " + out);
    }
    
    output.write((char*)&total, 4); // write total amount of content bits to read
    output.write((char*)&count, 4); // write the number of entries
    header_size += 8;
    
    header_size += head ? head->writeHeader(output) : 0; // write header
    
    file.clear();
    file.seekg(SEEK_SET);
    
    uint8_t buffer = 0;
    unsigned bit = 0;
    while (file.get(byte)) {
        content_size += nodes[byte]->writeCode(output, buffer, bit);
    }
    // write unfilled byte
    if (bit != 0) {
        output.write((char*)&buffer, 1);
        content_size++;
    }
    
    if (head) delete head;
    
    nodes.clear();
    sorted.clear();
    
    file.close();
    output.close();
    
    return {input_size, content_size, header_size};
}

std::tuple<int, int, int> Archiver::decode(std::string filename, std::string out) const {
    int output_size = 0, header_size = 0, content_size = 0;
    
    if (filename.empty() || out.empty()) {
        throw std::runtime_error("Filename can not be empty");
    }
    
    std::ifstream file;
    file.exceptions(std::ifstream::badbit); // throws on file exeptions
    file.open(filename, std::ios::in | std::ios::binary);
    
    if (!file.good()) {
        throw std::runtime_error("Could not open the file: " + filename);
    }
    
    // READ THE HEADER
    uint32_t total_bits;
    uint32_t num_entries;
    file.read((char*)&total_bits, 4);
    file.read((char*)&num_entries, 4);
    header_size += 8;
    
    Node *root = new Node();
    
    uint16_t entry_size;
    uint8_t byte;
    unsigned bit;
    for (int i = 0; i < num_entries; ++i) {
        file.read((char*)&entry_size, 2);
        header_size += 2;
        bit = 0;
        
        header_size += root->readEntry(file, entry_size, byte, bit);
    }
    
    // READ CONTENTS AND WRITE TO FILE
    std::ofstream output;
    output.exceptions(std::ifstream::badbit);
    output.open(out);
    
    if (!output.good()) {
        file.close();
        delete root;
        throw std::runtime_error("Could not open the file: " + out);
    }
    
    uint32_t total = 0; // total bits read
    bit = 0;
    // we need to handle empty files + don't want to write the last character
    while (file.peek() != std::ifstream::traits_type::eof() && total <= total_bits) {
        char symbol = root->readCode(file, byte, bit, total);
        output.write(&symbol, 1);
        output_size++;
//        symbol = root->readCode(file, byte, bit, total);
    }
    content_size = total_bits / 8 + (total_bits % 8 == 0 ? 0 : 1); // amount of bits / 8 + potentially unfilled byte
    
    delete root;
    file.close();
    output.close();
    
    return {content_size, output_size, header_size};
}


int Node::writeCode(std::ofstream &file, uint8_t &buffer, unsigned &bit) const {
    int total = 0;
    for (auto ch : this->code) {
        if (ch == '0') {
            buffer &= ~(1 << bit);
        } else {
            assert(ch == '1');
            buffer |= 1 << bit;
        }
        bit++;
        if (bit == 8) {
            file.write((char*)&buffer, 1);
            total++;
            buffer = 0;
            bit = 0;
        }
        assert(bit < 8);
    }
    return total;
}

int Node::writeHeader(std::ofstream &file) const {
    int total = 0; // total bytes written;
    auto l = this->left, r = this->right;
    if (l || r) {
        if (l) total += l->writeHeader(file);
        if (r) total += r->writeHeader(file);
    } else {
        // length -> code -> optional padding -> char
        uint16_t size = this->code.length();
        file.write((char*)&size, 2);
        total += 2;
        
        uint8_t buffer;
        unsigned bit = 0;
        total += this->writeCode(file, buffer, bit);
        // finish writing a byte
        if (bit != 0) {
            total++;
            file.write((char*)&buffer, 1);
        }
        
        if ((int)ceil(size / 8.0) % 2 == 0) {
            // add padding
            file.write("\0", 1);
            total++;
        }
        
        file.write(&this->byte, 1);
        total++;
    }
    return total;
}

Node::Node(char character, size_t count, Node *left, Node *right) {
    this->byte = character;
    this->frequency = count;
    this->left = left;
    this->right = right;
}

Node::~Node() {
    if (this->left)
        delete this->left;
    if (this->right)
        delete this->right;
}

int Node::readEntry(std::ifstream &file, uint16_t size, uint8_t &byte, unsigned &bit) {
    int total = 0; // total bytes read
    if (size == 0) {
        // reading is finished
        if ((int)ceil(this->code.length() / 8.0) % 2 == 0) {
            // padding was added, skip it
            file.seekg(1, file.cur);
            total++;
        }
        file.read(&this->byte, 1);
        total++;
        return total;
    }
    
    bit %= 8;
    if (bit == 0) {
        // byte is done, read new one
        file.read((char*)&byte, 1);
        total++;
    }
    
    Node *next;
    if (0x01 & (byte >> bit)) {
        // bit is 1
        if (!this->right) {
            this->right = new Node();
            this->right->code = this->code + '1';
        }
        next = this->right;
    } else {
        // bit is 0
        if (!this->left) {
            this->left = new Node();
            this->left->code = this->code + '0';
        }
        next = this->left;
    }
    
    total += next->readEntry(file, size - 1, byte, ++bit);
    return total;
}

char Node::readCode(std::ifstream &file, uint8_t &byte, unsigned &bit, uint32_t &total) const {
    if (file.eof()) return '\0';
    
    if (!this->right && !this->left) {
        return this->byte;
    }
    
    bit %= 8;
    if (bit == 0) {
        file.read((char*)&byte, 1);
    }
    
    if (0x01 & (byte >> bit)) {
        // bit is 1
        assert(this->right);
        total += 1;
        return this->right->readCode(file, byte, ++bit, total);
    } else {
        // bit is 0
        assert(this->left);
        total += 1;
        return this->left->readCode(file, byte, ++bit, total);
    }
}

bool Node::operator < (const Node &other) const {
    return this->frequency < other.frequency;
}

bool Node::operator == (const Node &other) const {
    bool left = (this->left && other.left && *this->left == *other.left) || (!this->left && !other.left);
    bool right = (this->right && other.right && *this->right == *other.right) || (!this->right && !other.right);
    
    return this->byte == other.byte && left && right;
}
