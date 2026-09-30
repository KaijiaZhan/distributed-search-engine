cmake_minimum_required(VERSION 3.20)

project(DistributedSearchEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    search_engine
    indexer/src/main.cpp
    indexer/src/tokenizer.cpp
    indexer/src/inverted_index.cpp
)

target_include_directories(
    search_engine
    PRIVATE
    indexer/include
)