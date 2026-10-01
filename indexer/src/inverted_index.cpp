#include "inverted_index.h"
#include <algorithm>

void InvertedIndex::addDocument(int documentId, const std::vector<std::string>& tokens) {

    documentLengths_[documentId] = static_cast<int>(tokens.size());

    totalDocumentLength_ += static_cast<int>(tokens.size());

    documentCount_++;

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

int InvertedIndex::getDocumentFrequency(const std::string& term) const{
    auto it = index_.find(term);

    if (it == index_.end()){
        return 0;
    }

    return static_cast<int>(it->second.size());
}

int InvertedIndex::getDocumentLength(int documentId) const{
    auto it = documentLengths_.find(documentId);

    if (it == documentLengths_.end()){
        return 0;
    }

    return it->second;
}

double InvertedIndex::getAverageDocumentLength() const {
    if (documentCount_ == 0){
        return 0.0;
    }

    return static_cast<double>(totalDocumentLength_) / documentCount_;
}