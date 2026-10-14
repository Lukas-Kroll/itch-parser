//
// Created by Lukas on 13.10.2026.
//

#ifndef ITCH_PARSER_AVXMTPARSER_HPP
#define ITCH_PARSER_AVXMTPARSER_HPP
#pragma once
#include "Parser.hpp"
#include <string>

class NaiveParser : public InterfaceParser
{
public:
    size_t parse(const std::string& filepath) override;
};
#endif //ITCH_PARSER_AVXMTPARSER_HPP
