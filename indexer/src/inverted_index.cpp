#include "inverted_index.h"
#include <algorithm>

void InvertedIndex::addDocument(int documentId, const std::vector<std::string>& tokens) {
    for (const auto& token : tokens){

        auto& documentIds = index_[token];

        auto it = std::find(documentIds.begin(), documentIds.end(), documentId);
        
        if (it == documentIds.end()){
            documentIds.push_back(documentId);
        }
    }
}

std::vector<int> InvertedIndex::search(const std::string& term) const {
    auto it = index_.find(term);
    if (it == index_.end()){
        return {};
    }

    return it->second;
}