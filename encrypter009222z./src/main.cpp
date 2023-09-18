#include <iostream>

#include "argh.h"
#include "huffman.hpp"

void print_usage() {
    std::cout << "Usage: ./hw_02 [-c | -u] [-f | --file] input [-o | --output] output\n";
}

int main(int argc, const char * argv[]) {
    argh::parser parser({"-f", "--file", "-o", "--output"});
    parser.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);
    
    cip::Archiver ar;
    
    if (parser["c"]) {
        try {
            std::string input = parser({"-f", "--file"}).str();
            std::string output = parser({"-o", "--output"}).str();
            auto [input_size, content_size, header_size] = ar.encode(input, output);
            std::cout << input_size << "\n" << content_size << "\n" << header_size << "\n";
        } catch (std::exception &e) {
            std::cerr << "Error occured: " << e.what() << "\n";
        }
    } else if (parser["u"]) {
        try {
            std::string input = parser({"-f", "--file"}).str();
            std::string output = parser({"-o", "--output"}).str();
            auto [content_size, output_size, header_size] = ar.decode(input, output);
            std::cout << content_size << "\n" << output_size << "\n" << header_size << "\n";
        } catch (std::exception &e) {
            std::cerr << "Error occured: " << e.what() << "\n";
        }
    } else {
        print_usage();
    }
    
    return 0;
}
