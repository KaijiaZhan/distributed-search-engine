#include "inverted_index.h"
#include <algorithm>

void InvertedIndex::addDocument(int documentId, const std::vector<std::string>& tokens) {
    for (const auto& token : tokens){

        auto& postings = index_[token];

        auto it = std::find_if(postings.begin(), postings.end(), [documentId](const Posting& posting){
            return posting.documentId == documentId;
        });

        if (it == postings.end()){
            Posting posting;
            posting.documentId = documentId;
            posting.frequency = 1;

            postings.push_back(posting);
        }
        else {
            it->frequency++;
        }
    }
}

std::vector<int> InvertedIndex::search(const std::string& term) const {
    auto it = index_.find(term);

    if (it == index_.end()){

        return {};
    }

    std::vector<int> documentIds;

    for (const auto& posting : it->second){
        documentIds.push_back(posting.documentId);
    }

    return documentIds;
}

std::vector<Posting> InvertedIndex::getPostings(const std::string& term) const {
    auto it = index_.find(term);

    if (it == index_.end()){
        return {};
    }

    return it->second;
}