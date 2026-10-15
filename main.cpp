#include <string>
#include <chrono>
#include <memory>
#include <filesystem>
#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <numeric>
#include <x86intrin.h>

#include "MTParser.hpp"
#include "NaiveParser.hpp"
#include "Parser.hpp"


int main(int argc, char* argv[])
{
    std::string filepath = "01302020.NASDAQ_ITCH50"; // default
    std::string version = "naive"; // default

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if ((arg == "-v") || (arg == "--version") && i + 1 < argc)
        {
            version = argv[i + 1];
            i++;
        }

        else if ((arg == "-f") || (arg == "--file") && i + 1 < argc)
        {
            filepath = argv[i + 1];
            i++;
        }
    }

    std::cout << "Selected Version: " << version << "\n";
    std::cout << "Target File: " << filepath << "\n";

    std::unique_ptr<InterfaceParser> parser;

    if (version == "mt")
    {
        parser = std::make_unique<MTParser>();
    }
    else if (version == "naive")
    {
        parser = std::make_unique<NaiveParser>();
    }
    //other versions:

    else
    {
        std::cerr << "Couldn't recognize version: " << version << "\n";
        return 1;
    }

    if (!std::filesystem::exists(filepath))
    {
        std::cerr << "Error: File does not exist at " << filepath << "\n";
        return 1;
    }

    uintmax_t fileSize = std::filesystem::file_size(filepath);
    double fileSizeMB = static_cast<double>(fileSize) / (1024 * 1024);

    constexpr int totalRuns = 11;
    std::vector<double> runTimesSeconds;
    runTimesSeconds.reserve(totalRuns-1);


    try
    {
        for (int run = 0; run < totalRuns; ++run)
        {
            auto start = std::chrono::high_resolution_clock::now();
            size_t parsedCount = parser->parse(filepath);
            std::cout << "Successfully parsed " << parsedCount << " messages.\n";
            auto end = std::chrono::high_resolution_clock::now();

            // Calculate Throughput

            std::chrono::duration<double, std::milli> elapsedMs = end - start;
            double seconds = elapsedMs.count() / 1000.0;
            double throughputMBs = fileSizeMB / seconds;

            if (run == 0)
            {
                std::cout << "[Warmup Run] Time: " << seconds << " s | Throughput: " << throughputMBs << " MiB/s\n";
            }
            else
            {
                std::cout << "[Run " << run << "]       Time: " << seconds << " s | Throughput: " << throughputMBs << " MiB/s\n";
                runTimesSeconds.push_back(seconds);
            }
        }


        double avgSeconds = std::accumulate(runTimesSeconds.begin(), runTimesSeconds.end(), 0.0) / runTimesSeconds.size();
        double avgThroughput = fileSizeMB / avgSeconds;

        std::cout << "\n========================================\n";
        std::cout << "          BENCHMARKING RESULTS         \n";
        std::cout << "========================================\n";
        std::cout << "File size:      " << fileSizeMB << " MB\n";
        std::cout << "Avg Time taken: " << avgSeconds << " s\n";
        std::cout << "Avg Throughput: " << avgThroughput << " MB/s\n";
        std::cout << "========================================\n";


    } catch (const std::exception& e)
    {
        std::cerr << "Failed to parse: " << e.what() << std::endl;
        return 1;
    }


    return 0;
}
