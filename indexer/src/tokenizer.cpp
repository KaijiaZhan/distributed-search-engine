#include "tokenizer.h"
#include <algorithm> 
/*Algorithm will allow us to process a large dataset and process the chracters*/
/*would it be better if I wrote my own algorithm?*/
#include <cctype>
/*cctype provides operations we can perform on those characters; lowercase/check individual characters*/
#include <sstream>
/*Let's us treat a string like a stream of data; split cleaned text into words*/

std::vector<std::string> Tokenizer::tokenize(const std::string& text){
    std::string cleaned = text;

    std::transform(
        cleaned.begin(),
        cleaned.end(),
        cleaned.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        }
    );
    /*Visits every character; Lambda receives character; Converts to lower; Store lowercase back into cleaned*/

    for (char& c : cleaned) {
        if (!std::isalnum(static_cast<unsigned char>(c))) {
            c = ' ';
        }
    }
    /*Replace non-numeric or alphabetic characters with a whitespace*/

    std::stringstream stream(cleaned);
    /*Treats whitespace like separators and doesn't matter if we have multiple spaces*/

    std::vector<std::string> tokens;
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;

}
