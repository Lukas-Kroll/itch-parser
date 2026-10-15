//
// Created by Lukas on 13.10.2026.
//
//
// Created by Lukas on 11.10.2026.
//


#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <filesystem>
#include <thread>
#include <x86intrin.h>

//my own headers:
#include "ringbuffer.hpp"
#include "messages.h"
#include "message_information.h"
#include "NaiveParser.hpp"
#include "MappedFile.hpp"


size_t NaiveParser::parse(const std::string& filepath)
{

    MappedFile file (filepath);

    const uint8_t* current = file.data();
    const uint8_t* end = current + file.size();



    size_t messageCounter   = 0;

    while (current < end)
    {
        uint8_t sizeByte = current[1];
        Message_Information msg_information = {current + 2, current[2]};

        //consume orderbook dummy
        messageCounter++;

        current+= sizeByte + 2;
    }

    return messageCounter;
}
