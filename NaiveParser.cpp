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


size_t NaiveParser::parse(const std::string& filepath)
{
   int fileDescriptor = open(filepath.c_str(), O_RDONLY);
    if (fileDescriptor == -1)
    {
        throw std::runtime_error("ERROR! Couldn't open file at: " + filepath);
    }

    size_t fileSize = std::filesystem::file_size(filepath);

    void* filePointer = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);

    if (filePointer == MAP_FAILED)
    {
        close(fileDescriptor);
        throw std::runtime_error("ERROR! couldn't map memory!");
    }


    const uint8_t* cursor = static_cast<const uint8_t*> (filePointer);
    const uint8_t* end = cursor + fileSize;


    const uint8_t* current = cursor;
    size_t messageCounter   = 0;

    while (current < end)
    {
        uint8_t sizeByte = current[1];
        Message_Information msg_information = {current + 2, current[2]};

        //consume orderbook dummy
        messageCounter++;

        current+= sizeByte + 2;
    }
    //cleanup

    munmap(filePointer, fileSize);
    close(fileDescriptor);
    return messageCounter;
}
