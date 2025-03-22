cmake_minimum_required(VERSION 3.1...3.10)

project(website)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-language-extension-token")

option(ENABLE_SANITIZERS "Enable Address and UB sanitizers" ON)

if (ENABLE_SANITIZERS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")
endif ()

add_subdirectory(mew)
add_subdirectory(website)
