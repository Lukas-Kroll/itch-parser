# NASDAQ ITCH 5.0 High-Performance Binary Parser

A high-performance, zero-copy C++ parser for the **NASDAQ ITCH 5.0** market data protocol. This project explores and benchmarks architectural tradeoffs between a raw single-threaded baseline (`mmap`) and a multi-threaded lock-free producer-consumer architecture using SPSC ring buffers.

---


## 🎯 Motivation & Background

I built this project to explore how to efficiently parse massive multi-gigabyte binary datasets in systems programming. Financial market data feeds like NASDAQ ITCH 5.0 require handling hundreds of millions of variable-length packets under tight performance constraints. I wanted to build a custom parser from scratch, comparing a raw, memory-mapped single-threaded baseline against a cache-aligned, lock-free multi-threaded ring buffer architecture.

## 🚀 Key Architectural Features

* **Zero-Copy Memory-Mapped I/O:** Utilizes `mmap` to ingest multi-gigabyte binary datasets directly from disk without redundant user-space buffering.
* **Polymorphic Design:** Clean interface-based contract (`InterfaceParser`) allowing seamless runtime polymorphism between execution models.
* **Lock-Free Multi-Threading (SPSC Ring Buffer):** Implements a cache-aligned, lock-free Single-Producer Single-Consumer ring buffer per worker thread to avoid heavy mutex contention.
* **Local Staging Batching:** Minimizes atomic overhead and cache-line bouncing by staging messages in local L1/L2 cache arrays before bulk pushing to ring buffers.
* **Rigorous Benchmarking:** Built-in `std::chrono` multi-run benchmarking with warmup segregation and average throughput calculation.

---

## 🖥️ Test Environment & Hardware

The benchmarks were conducted on a **TUM departmental Linux server**:
* **CPU:** AMD EPYC 9554P 64-Core Processor
* **Compiler:** GCC 13.3.0 (with `-O3` Release optimizations)
* **Build System:** CMake 3.28.3

---

## 📊 Benchmark Results & Analysis

Tested on a **12.35 GB** raw NASDAQ ITCH 5.0 dataset (`01302020.NASDAQ_ITCH50`) containing over **423 million messages**.

| Parser Architecture | Avg. Time Taken | Throughput (MB/s) | Architectural Characteristics |
| :--- | :--- | :--- | :--- |
| **NaiveParser** (Single-Threaded) | **1.48 s** | **8,365.88 MB/s** | Sequential linear scan; leverages hardware prefetching; zero synchronization overhead. |
| **MTParser** (Multi-Threaded / Batching) | **6.97 s** | **1,773.01 MB/s** | 4-worker pipeline; distributes load via ring buffers; incurs atomic/scheduling overhead for lightweight payloads. |

### 💡 Core Engineering Insight: Why Naive Outperformed MT Here
When parsing raw streams where the per-message payload workload is extremely lightweight (e.g., header skipping and sequence counting), the sequential **`NaiveParser`** dominates. The CPU's hardware prefetchers seamlessly stream contiguous memory into cache, achieving near-memory-bandwidth speeds (~8.36 GB/s). 

The multi-threaded **`MTParser`** introduces thread scheduling, atomic memory barriers, and ring buffer tracking. While its throughput of **1.76 GB/s** is phenomenal for a multi-core design, it acts as a reminder that synchronization overhead only pays off when the consumer performs heavy computation per message (such as maintaining a live limit order book).

---

## 🛠️ Tech Stack & Requirements

* **Language:** C++20
* **Build System:** CMake (>= 3.28)
* **Compiler:** GCC (>= 13.3.0) / Linux (POSIX `mmap`, `unistd.h`)

---

## ⚙️ Building & Running

> ⚠️ **Note on Data:** Due to file size constraints (12.35 GB), the raw NASDAQ ITCH 5.0 binary file is **not** included in this repository. To run the benchmarks, you must supply your own ITCH 5.0 binary data file (e.g., from Nasdaq's data distribution services or a custom sample).

Clone the repository and build in Release mode:

```bash
# Clone repository
git clone https://github.com/Lukas-Kroll/itch-parser.git
cd itch-parser

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build-debug
cmake --build cmake-build-debug --parallel 4

# Run Naive Parser baseline with your file path
./cmake-build-debug/ITCH_Parser -v naive -f /path/to/your/itch_file

# Run Multi-Threaded Parser
./cmake-build-debug/ITCH_Parser -v mt -f /path/to/your/itch_file
