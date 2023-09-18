#ifndef huffman_hpp
#define huffman_hpp

#include <iostream>

namespace cip {
    
    class Archiver {
        
    public:
        std::tuple<int, int, int> encode(std::string input_file, std::string output_file) const;
        std::tuple<int, int, int> decode(std::string input_file, std::string output_file) const;
    };
    
    class Node {
        char byte;
        size_t frequency;
        std::string code;
        Node *left, *right;
        
        void supplyCode(std::string prefix);
        int writeHeader(std::ofstream &file) const;
        int writeCode(std::ofstream &file, uint8_t &buffer, unsigned &bit) const;
        
        int readEntry(std::ifstream &file, uint16_t size, uint8_t &byte, unsigned &bit);
        char readCode(std::ifstream &file, uint8_t &byte, unsigned &bit, uint32_t &total) const;
        
        friend class Archiver;
    public:
        Node(char character = '\0', size_t count = 0, Node *left = nullptr, Node *right = nullptr);
        ~Node();
        
        bool operator < (const Node& other) const;
        bool operator == (const Node& other) const;
    };
}

#endif /* huffman_hpp */
