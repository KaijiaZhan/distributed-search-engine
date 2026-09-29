#pragma once
/*means we are only compiling this once*/
#include <string>
#include <vector>

class Tokenizer {
    public: 
        std::vector<std::string> tokenize(const std::string& text);

};