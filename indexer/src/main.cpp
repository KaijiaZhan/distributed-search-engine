#include "tokenizer.h"
#include "document.h"
#include "inverted_index.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>

struct SearchResult {
    int documentId;
    double score;
};

int main () {
    Document document;
    document.id = 1;
    document.title = "Introduction to Distributed Systems";
    document.content = "Distributed systems use multiple computers to solve problems.";

    Document document2;
    document2.id = 2;
    document2.title = "Introduction to Programming";
    document2.content = "Computers execute programs written by developers. Computers are powerful.";

    std::vector<Document> documents = {document, document2};

    Tokenizer tokenizer;

    InvertedIndex index;

    for (const auto& doc : documents){
        auto tokens = tokenizer.tokenize(doc.content);
        
        index.addDocument(doc.id, tokens);
    }

    std::string searchTerm;

    std::cout << "Enter a search term: ";
    std::getline(std::cin, searchTerm);

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

    std::cout << "Search results for " << searchTerm << ":" << std::endl;

    if (scoredResults.empty()){
        std::cout << "No results found." << std::endl;
    }

    for (const auto& result : scoredResults) {
        for (const auto& doc : documents) {
            if (doc.id == result.documentId){
                std::cout << doc.title << std::endl;
                break;
            }
        }
    }

    return 0;
}