cmake_minimum_required(VERSION 3.1...3.10)

project(website)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

if(MSVC OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC"))
  # MSVC or clang-cl flags
  add_compile_options(/W4)
else()
  # GCC/Clang flags
  set(CMAKE_C_FLAGS "-g -DLOG_WITH_FILE -Wall -Wextra -pedantic -Wno-language-extension-token")
  
  # Sanitizer options
  option(ENABLE_SANITIZERS "Enable Address and UB sanitizers" ON)

  if (ENABLE_SANITIZERS)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")
  endif()
endif()

add_subdirectory(mew)
add_subdirectory(website)
