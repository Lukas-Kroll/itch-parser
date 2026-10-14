//
// Created by Lukas on 11.10.2026.
//

#ifndef ITCH_PARSER_V1_HPP
#define ITCH_PARSER_V1_HPP
#pragma once
#include "Parser.hpp"
#include <string>

class MTParser : public InterfaceParser
{
public:
    size_t parse(const std::string& filepath) override;
};

#endif //ITCH_PARSER_V1_HPP
