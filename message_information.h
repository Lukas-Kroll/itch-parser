//
// Created by Lukas on 02.06.2026.
//
#pragma once
#ifndef ITCH_PARSER_MESSAGE_INFORMATION_H
#define ITCH_PARSER_MESSAGE_INFORMATION_H
#include <cstdint>

struct Message_Information
{
    const uint8_t* messagePointer;
    uint8_t messageType;

};

#endif //ITCH_PARSER_MESSAGE_INFORMATION_H
