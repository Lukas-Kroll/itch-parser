cmake_minimum_required(VERSION 3.28.3)
project(ITCH_Parser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-O3 -march=native -Wall -Wextra)
endif ()

add_executable(ITCH_Parser main.cpp messages.h
        message_information.h
        ringbuffer.hpp
        MTParser.cpp
        MTParser.hpp
        Parser.hpp
        NaiveParser.hpp
        NaiveParser.cpp
)

if (MSVC)
    target_compile_options(ITCH_Parser PRIVATE /arch:AVX2)
else ()
    target_compile_options(ITCH_Parser PRIVATE -mavx2)
endif ()