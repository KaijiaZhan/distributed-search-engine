#include "tokenizer.h"
#include "document.h"
#include "inverted_index.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

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

    std::vector<int> results = index.search(queryTokens[0]);

    for (size_t i = 1; i < queryTokens.size(); i++){
        auto tokenResults = index.search(queryTokens[i]);

        std::vector<int> intersection;

        for (const auto& documentId : results){
            auto it = std::find(tokenResults.begin(), tokenResults.end(), documentId);

            if (it != tokenResults.end()){
                intersection.push_back(documentId);
            }
        }

        results = std::move(intersection);

        if (results.empty()){
            break;
        }
    }

    std::cout << "Search results for " << searchTerm << ":" << std::endl;

    if (results.empty()){
        std::cout << "No results found." << std::endl;
    }

    for (const auto& documentId : results) {
        for (const auto& doc : documents) {
            if (doc.id == documentId){
                std::cout << doc.title << std::endl;
                break;
            }
        }
    }

    return 0;
}