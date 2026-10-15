cmake_minimum_required(VERSION 3.28.3)
project(ITCH_Parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(ITCH_Parser main.cpp messages.h
        message_information.h
        ringbuffer.hpp
        MTParser.cpp
        MTParser.hpp
        Parser.hpp
        NaiveParser.hpp
        NaiveParser.cpp
        MappedFile.hpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ITCH_Parser PRIVATE -Wall -Wextra)
endif()