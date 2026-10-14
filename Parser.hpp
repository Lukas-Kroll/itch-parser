//
// Created by Lukas on 12.10.2026.
//



#ifndef ITCH_PARSER_PARSER_HPP
#define ITCH_PARSER_PARSER_HPP
#pragma once
#include <string>

class InterfaceParser
{
    public:
        virtual size_t parse (const std::string& filepath) = 0;
        virtual ~InterfaceParser() = default;
};


#endif //ITCH_PARSER_PARSER_HPP
