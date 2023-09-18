// Doctest will implement `main` on its own

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "huffman_test.hpp"

//int main(int argc, char *argv[]) {
//    doctest::Context context;
//    context.applyCommandLine(argc, argv);
//
//    ArchiverTest test;
//
//    int res = context.run(); // run
//
//    if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
//        return res;
//
//    test.test_text();
//    test.test_big();
//
//    return 0;
//}
