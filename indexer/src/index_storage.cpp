#include "index_storage.h"
#include <fstream>
#include <stdexcept>

void IndexStorage::save(const InvertedIndex& index, const std::string& filePath) {
    std::ofstream file(filePath);

    if (!file.is_open()){
        return;
    }

    file << index.index_.size() << '\n';

    for (const auto& entry : index.index_){
        file << entry.first << " " << entry.second.size() << '\n';

        for (const auto& posting : entry.second) {
            file << posting.documentId << " " << posting.frequency << '\n';
        }
    }

    file << index.documentLengths_.size() << '\n';

    for (const auto& entry : index.documentLengths_) {
        file << entry.first << " " << entry.second << '\n';
    }

    file << index.totalDocumentLength_ << '\n';

    file << index.documentCount_ << '\n';
}

InvertedIndex IndexStorage::load(const std::string& filePath){
    InvertedIndex index;

    std::ifstream file(filePath);

    if (!file.is_open()){
        throw std::runtime_error("Failed to open index file: " + filePath);
    }

    size_t termCount = 0;
    if (!(file >> termCount)) {
        throw std::runtime_error("Corrupted index file: invalid term count.");
    }

    for (size_t i = 0; i <termCount; i++){
        std::string term;
        size_t postingCount = 0;

        file >> term >> postingCount;

        for (size_t j = 0; j <postingCount; j++){
            Posting posting;

            if (!(file >> posting.documentId >> posting.frequency)) {
                throw std::runtime_error("Corrupted index file: invalid posting data.");
            }

            if (posting.documentId <= 0 || posting.frequency <= 0) {
                throw std::runtime_error("Corrupted index file: invalid document ID or frequency.");
            }

            index.index_[term].push_back(posting);

        }
    }

    size_t documentLengthCount = 0;
    
    if (!(file >> documentLengthCount)) {
        throw std::runtime_error("Corrupted index file: invalid document length count.");
    }

    for (size_t i = 0; i < documentLengthCount; i++){
        int documentId = 0;
        int documentLength = 0;

        if (!(file >> documentId >> documentLength)) {
            throw std::runtime_error("Corrupted index file: invalid document length data.");
        }

        if (documentId <= 0 || documentLength < 0) {
            throw std::runtime_error("Corrupted index file: invalid document ID or length.");
        }

        index.documentLengths_[documentId] = documentLength;
    }

    if (!(file >> index.totalDocumentLength_)) {
    throw std::runtime_error("Corrupted index file: invalid total document length.");
}

    if (!(file >> index.documentCount_)) {
        throw std::runtime_error("Corrupted index file: invalid document count.");
    }

    if (index.totalDocumentLength_ < 0 || index.documentCount_ <= 0) {
        throw std::runtime_error("Corrupted index file: invalid index metadata.");
    }

    if (static_cast<size_t>(index.documentCount_) != index.documentLengths_.size()) {
        throw std::runtime_error("Corrupted index file: document count mismatch.");
    }

    return index;
}