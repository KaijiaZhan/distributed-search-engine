#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Posting {
    int documentId;
    int frequency;
};

class InvertedIndex {
    public:
        void addDocument(int documentId, const std::vector<std::string>& tokens);
        std::vector<int> search(const std::string& term) const;
        std::vector<Posting> getPostings(const std::string& term) const;
    private:
        std::unordered_map<std::string, std::vector<Posting>> index_;
};