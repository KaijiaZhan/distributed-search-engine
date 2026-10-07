#include "document_loader.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<Document> DocumentLoader::loadDocuments(const std::string& directoryPath){
    std::vector<Document> documents;

    int nextDocumentId = 1;

    std::vector<std::filesystem::path> filePaths;

    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
        return documents;
    }


    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)){


        if (!entry.is_regular_file()){

            continue;
        }

        auto filePath = entry.path();

        if (filePath.extension() != ".txt"){
            continue;
        }

        filePaths.push_back(filePath);

    }

    std::sort(filePaths.begin(), filePaths.end());

    for (const auto& filePath: filePaths){

        std::ifstream file(filePath);

        if (!file.is_open()){
            continue;
        }

        std::stringstream buffer;

        buffer << file.rdbuf();

        std::string content = buffer.str();

        Document document;

        document.content = content;
        document.title = filePath.stem().string();
        document.id = nextDocumentId;
        nextDocumentId++;

        documents.push_back(document);
    }

    return documents;

}