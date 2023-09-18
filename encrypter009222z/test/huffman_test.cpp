#include "huffman_test.hpp"

#include <fstream>

static bool files_equal(const std::string& p1, const std::string& p2) {
    std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
      return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
      return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
  }

static const int argh_file_size = 18834;
static const int argh_compressed_file_size = 11665;

static const int binary_file_size = 1316108;
static const int binary_compressed_file_size = 1315041;

static const int big_file_size = 6795789;
static const int big_file_compressed_size = 3020449;

void ArchiverTest::test_encoding(int file_size, int compressed_file_size, std::string input, std::string output) {
    auto [input_size, content_size, header_size] = ar.encode(input, output);
    CHECK(input_size == file_size);
    CHECK(content_size + header_size == compressed_file_size);
    encoded_file_size = input_size;
    encoded_header_size = header_size;
    encoded_content_size = content_size;
}

void ArchiverTest::test_decoding(std::string input, std::string output, std::string original) {
    auto [content_size, output_size, header_size] = ar.decode(input, output);
    CHECK(content_size == encoded_content_size);
    CHECK(output_size == encoded_file_size);
    CHECK(header_size == encoded_header_size);
    CHECK(files_equal(original, output));
}

// Time lime 5 seconds (encoding and decoding bots happens in that time)
TEST_SUITE("Archiver tests" * doctest::timeout(5)) {
    
    TEST_CASE_FIXTURE(ArchiverTest, "Archive and unarchive a medium text file") {
        test_encoding(argh_file_size, argh_compressed_file_size, "src/argh.h", "gen/argh.bin");
        test_decoding("gen/argh.bin", "gen/argh.h", "src/argh.h");
    }
    
    TEST_CASE_FIXTURE(ArchiverTest, "Archive and unarchive a medium binary file") {
        test_encoding(binary_file_size, binary_compressed_file_size, "samples/binary.jpg", "gen/binary.bin");
        test_decoding("gen/binary.bin", "gen/binary.jpg", "samples/binary.jpg");
    }
    
    TEST_CASE_FIXTURE(ArchiverTest, "Archive and unarchive an empty file") {
        test_encoding(0, 8, "samples/empty", "gen/empty.bin");
        test_decoding("gen/empty.bin", "gen/empty", "samples/empty");
    }
    
    TEST_CASE_FIXTURE(ArchiverTest, "Archive and unarchive a big file") {
        test_encoding(big_file_size, big_file_compressed_size, "samples/big.txt", "gen/big.bin");
        test_decoding("gen/big.bin", "gen/big.txt", "samples/big.txt");
    }
    
    TEST_CASE_FIXTURE(ArchiverTest, "Check error handling") {
        REQUIRE_THROWS(test_encoding(0, 0, "does not exist", "gen/should_not_be_here.bin"));
    }
}


;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;        ;;;;;;;;;
;;;;;;;            ;;;;;;;
;;;;;                ;;;;;
;;;;;                ;;;;;
;;;;;                ;;;;;
;;;;;                ;;;;;
;;;;;                ;;;;;

;; /* ARC OF THE DOOOm*/ ;;
