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
#include <array>
#include <x86intrin.h>
#include <memory>
//my own headers:
#include "MTParser.hpp"
#include "ringbuffer.hpp"
#include "messages.h"
#include "message_information.h"
#include "MappedFile.hpp"

size_t MTParser::parse(const std::string& filepath)
{
    MappedFile file (filepath);

    const uint8_t* cursor = file.data();
    const uint8_t* end = cursor + file.size();

    constexpr std::size_t NUM_OF_Workers = 4;
    constexpr std::size_t BATCH_SIZE = 32;


    auto stockBuffer = std::make_unique<ringbuffer<Message_Information, 65536>[]>(NUM_OF_Workers);


    //check if producer finished:
    std::atomic<bool> producerDone{false};

    //dummy counter
    std::array<std::size_t, NUM_OF_Workers> workerMessageCounts{0};

    //start producerThread


    std::thread producer([cursor, end, &stockBuffer, &producerDone] () {
        const uint8_t* current = cursor;

        std::array<Message_Information, BATCH_SIZE> localBatches[NUM_OF_Workers];
        std::size_t batchCounts[NUM_OF_Workers] = {0};

        while (current < end)
        {
            uint8_t sizeByte = current[1];
            std::size_t workerId = current[4] % NUM_OF_Workers;

            localBatches[workerId][batchCounts[workerId]++] = {current + 2, current[2]};

            if (batchCounts[workerId] == BATCH_SIZE)
            {
                while (!stockBuffer[workerId].push_batch(localBatches[workerId].data(), BATCH_SIZE))
                {
                    _mm_pause();
                }
                batchCounts[workerId] = 0;
            }

            current += sizeByte + 2;
        }

        for (std::size_t i = 0; i < NUM_OF_Workers; ++i)
        {
            if (batchCounts[i] > 0)
            {
                while (!stockBuffer[i].push_batch(localBatches[i].data(), batchCounts[i]))
                {
                    _mm_pause();
                }
            }
        }
        producerDone.store(true, std::memory_order_release);
    });

    //start consumer threads:
    std::array<std::thread, NUM_OF_Workers> workerArray;

    for (std::size_t i = 0; i < NUM_OF_Workers; i++)
    {
        workerArray[i] = std::thread([i, &producerDone, &stockBuffer, &workerMessageCounts] ()
        {
            std::array<Message_Information, BATCH_SIZE> batch;
            std::size_t localCount = 0;
            while (true)
            {
                std::size_t popped = stockBuffer[i].pop_batch(batch.data(), BATCH_SIZE);
                if (popped > 0)
                {
                    for (std::size_t j = 0; j < popped; ++j)
                    {
                        //consume orderbook
                        //asm volatile("": "+r" (msg));
                        localCount++;
                    }
                }
                else
                {
                    if (producerDone.load(std::memory_order_acquire))
                    {
                        popped = stockBuffer[i].pop_batch(batch.data(), BATCH_SIZE);
                        if (popped == 0)
                        {
                            break;
                        }
                        for (std::size_t j = 0; j < popped; ++j)
                        {
                            //consume orderbook dummy
                            localCount++;
                        }
                    }
                    else
                    {
                        _mm_pause();
                    }

                }
            }
            workerMessageCounts[i] = localCount;
        });
    }

    //Join Threads

    producer.join();

    for (auto& worker : workerArray)
    {
        worker.join();
    }


    size_t total_messages = 0;
    for (std::size_t count : workerMessageCounts)
    {
        total_messages += count;
    }

    return total_messages;
}
