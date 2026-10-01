#pragma once
#include "document.h"
#include <vector>

class DocumentLoader {
    public:
        std::vector<Document> loadDocuments(const std::string& directoryPath);        

};