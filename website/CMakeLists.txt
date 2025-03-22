cmake_minimum_required(VERSION 3.1...3.10)

project(website)

add_executable(server ./src/main.c)

target_include_directories(server
    PRIVATE ./src
)

target_link_libraries(server
    PRIVATE mew
)
