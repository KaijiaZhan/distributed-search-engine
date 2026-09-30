#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class InvertedIndex {
    public:
        void addDocument(int documentId, const std::vector<std::string>& tokens);
        std::vector<int> search(const std::string& term) const;
    private:
        std::unordered_map<std::string, std::vector<int>> index_;
};