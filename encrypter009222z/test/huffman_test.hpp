#ifndef huffman_test_hpp
#define huffman_test_hpp

#include "huffman.hpp"
#include "doctest.h"

class ArchiverTest {
    cip::Archiver ar;
    int encoded_file_size = 0, encoded_header_size = 0, encoded_content_size = 0;
public:
    void test_encoding(int expected_file_size,
                       int expected_compressed_file_size,
                       std::string input_filename,
                       std::string output_filename);
    
    void test_decoding(std::string input_filename, std::string output_filename, std::string origin_filename);
        
    void test_text();
    void test_binary();
    void test_empty();
    void test_big();
};

#endif /* huffman_test_hpp */
