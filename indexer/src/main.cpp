#include "tokenizer.h"
#include <iostream>

int main () {
    Tokenizer tokenizer;

    std::vector<std::string> tokens = tokenizer.tokenize("Hello, World! C++ is awesome in 2026.");

    for (const std::string& token: tokens){
        std::cout << token << std::endl;
    }

    return 0;
}