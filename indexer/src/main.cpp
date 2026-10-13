#include "tokenizer.h"
#include "document.h"
#include "inverted_index.h"
#include "document_loader.h"
#include "index_storage.h"
#include "document_fingerprint.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <exception>
#include <stdexcept>
#include <fstream>

struct SearchResult {
    int documentId;
    double score;
};

int main () {

    DocumentLoader loader;
    auto documents = loader.loadDocuments("../generated_documents");

    std:size_t documentFingerprint = calculateDocumentFingerprint("../generated_documents");
    std::cout << "Document fingerprints: " << documentFingerprint << std::endl;

    if (documents.empty()){
        std::cout << "No documents found." << std::endl;
        return 1;
    }

    Tokenizer tokenizer;
    InvertedIndex index;
    IndexStorage storage;

    bool indexExists = std::filesystem::exists("index.txt");

    bool documentsChanged = false;

    if (indexExists) {
        std::ifstream fingerprintFile("index.fingerprint");

        std::size_t savedFingerprint = 0;

        if (!(fingerprintFile >> savedFingerprint)){
            std::cout << "Saved fingerprint missing or invalid." << std::endl;
            documentsChanged = true; 
        }
        else if (savedFingerprint != documentFingerprint) {
            std::cout << "Document fingerprint changed." << std::endl;
            documentsChanged = true;
        }
    }

    if (indexExists) {
        auto indexTime = std::filesystem::last_write_time("index.txt");

        for (const auto& entry : std::filesystem::directory_iterator("../generated_documents")) {
            if (entry.is_regular_file()) {
                auto documentTime = entry.last_write_time();

                if (documentTime > indexTime) {
                    documentsChanged = true;
                    break;
                }
            }
        }
    }

    auto indexStart = std::chrono::high_resolution_clock::now();

    if (indexExists && !documentsChanged) {
        try {
            std::cout << "Loading existing index..." << std::endl;
            index = storage.load("index.txt");

            if (index.getDocumentCount() != static_cast<int>(documents.size())) {
                throw std::runtime_error("Document count changed. Rebuilding index.");
            }
        }
        catch (const std::exception& e) {
            std::cout << "Failed to load index: " << e.what() << std::endl;
            std::cout << "Rebuilding index..." << std::endl;

            index = InvertedIndex();

            for (const auto& doc : documents) {
                auto tokens = tokenizer.tokenize(doc.content);
                index.addDocument(doc.id, tokens);
            }

            storage.save(index, "index.txt");

            std::ofstream fingerprintFile("index.fingerprint");

            if (!fingerprintFile) {
                throw std::runtime_error("Failed to open fingerprint file for writing.");
            }

            fingerprintFile << documentFingerprint << '\n';

            if (!fingerprintFile) {
                throw std::runtime_error("Failed to save document fingerprint");
            }


            indexExists = false;
        }
    } else {
        std::cout << "Building new index..." << std::endl;

        for (const auto& doc : documents) {
            auto tokens = tokenizer.tokenize(doc.content);
            index.addDocument(doc.id, tokens);
        }

        storage.save(index, "index.txt");

        std::ofstream fingerprintFile("index.fingerprint");

        if (!fingerprintFile) {
            throw std::runtime_error("Failed to open fingerprint file for writing.");
        }

        fingerprintFile << documentFingerprint << '\n';

        if (!fingerprintFile) {
            throw std::runtime_error("Failed to save document fingerprint");
        }
    }

    auto indexEnd = std::chrono::high_resolution_clock::now();

    auto indexDuration = std::chrono::duration_cast<std::chrono::microseconds>(indexEnd - indexStart);

    std::cout << ((indexExists && !documentsChanged) ? "Index loaded in " : "Index built in ") << indexDuration.count() << " microseconds." << std::endl;
    std::string searchTerm;

    std::cout << "Enter a search term: ";
    std::getline(std::cin, searchTerm);

    auto searchStart = std::chrono::high_resolution_clock::now();

    auto queryTokens = tokenizer.tokenize(searchTerm);

    if (queryTokens.empty()){
        std::cout << "Please enter a valid search term." << std::endl;
        return 0;
    }

    std::vector<SearchResult> scoredResults;

    for (const auto& queryToken: queryTokens){
        auto postings = index.getPostings(queryToken);

        int documentFrequency = index.getDocumentFrequency(queryToken);

        if (documentFrequency == 0){
            continue;
        }

        double queryIdf = std::log(
            static_cast<double>(documents.size() + 1)/(documentFrequency + 1)
        ) + 1;

        for (const auto& posting : postings) {

            int documentLength = index.getDocumentLength(posting.documentId);

            double averageDocumentLength = index.getAverageDocumentLength();

            if (averageDocumentLength == 0.0 ){
                continue;
            }

            double lengthRatio = static_cast<double>(documentLength) / averageDocumentLength;

            double b = 0.75;

            double k1 = 1.5;

            double lengthNormalization = (1.0 - b) + (b * lengthRatio);

            double bm25Tf = (posting.frequency * (k1 + 1.0)) / (posting.frequency + (k1 * lengthNormalization));

            auto existingResult = std::find_if(
                scoredResults.begin(), 
                scoredResults.end(), 
                [posting](const SearchResult& result){
                    return result.documentId == posting.documentId;
            });

            if (existingResult != scoredResults.end()) {
                existingResult->score += bm25Tf * queryIdf;
            }
            else {
                SearchResult result;
                result.documentId = posting.documentId;
                result.score = bm25Tf * queryIdf;

                scoredResults.push_back(result);
            }
        }
    }

    std::vector<int> matchingDocumentIds = index.search(queryTokens[0]);

    for (size_t i = 1; i < queryTokens.size(); i++){
        auto tokenResults = index.search(queryTokens[i]);

        std::vector<int> intersection;

        for (const auto& documentId : matchingDocumentIds){

            auto it = std::find(tokenResults.begin(), tokenResults.end(), documentId);

            if (it != tokenResults.end()){
                intersection.push_back(documentId);
            }
        }

        matchingDocumentIds = std::move(intersection);

        if (matchingDocumentIds.empty()){
            break;
        }
    }

    scoredResults.erase(
        std::remove_if(
            scoredResults.begin(),
            scoredResults.end(),
            [&matchingDocumentIds](const SearchResult& result){
                return std::find(
                    matchingDocumentIds.begin(),
                    matchingDocumentIds.end(),
                    result.documentId
                ) == matchingDocumentIds.end();
            }
        ),
        scoredResults.end()
    );

    std::sort(
        scoredResults.begin(), 
        scoredResults.end(), 
        [](const SearchResult& a, const SearchResult& b){
            return a.score > b.score;
        }
    );

    auto searchEnd = std::chrono::high_resolution_clock::now();

    auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEnd - searchStart);

    std::cout << "Search completed in " << searchDuration.count() << " microseconds." << std::endl;

    std::cout << "Search results for " << searchTerm << ":" << std::endl;

    std::cout << "Total matches: " << scoredResults.size() << std::endl;

    if (scoredResults.empty()){
        std::cout << "No results found." << std::endl;
    }

    size_t resultsToShow = std::min(
        scoredResults.size(),
        static_cast<size_t>(10)
    );

    for (size_t i = 0; i < resultsToShow; i++) {
        const auto& result = scoredResults[i];

        for (const auto& doc : documents) {
            if (doc.id == result.documentId){
                std::cout << doc.title << std::endl;
                break;
            }
        }
    }

    return 0;
}