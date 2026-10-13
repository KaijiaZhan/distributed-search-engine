#pragma once
#include "inverted_index.h"
#include <string>

class IndexStorage {
    public:
        void save(const InvertedIndex& index, const std::string& filePath);
        InvertedIndex load(const std::string& filePath);
};