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
        friend class IndexStorage;
        void addDocument(int documentId, const std::vector<std::string>& tokens);
        std::vector<int> search(const std::string& term) const;
        std::vector<Posting> getPostings(const std::string& term) const;
        int getDocumentFrequency(const std::string& term) const;
        int getDocumentLength(int documentId) const;
        double getAverageDocumentLength() const;
        int getDocumentCount() const {
            return documentCount_;
        }
        
    private:
        std::unordered_map<std::string, std::vector<Posting>> index_;
        std::unordered_map<int, int> documentLengths_;
        int totalDocumentLength_ = 0;
        int documentCount_ = 0;
};