# NASDAQ ITCH 5.0 Parser: Single-Threaded vs. Sharded Lock-Free Pipeline

A C++20 implementation of a message-framing parser for the NASDAQ ITCH 5.0 market data format, built to study how far a sequential memory-mapped scan can go and when a multi-threaded SPSC ring-buffer pipeline starts to pay off.

Two interchangeable implementations behind one interface:

| Variant | Idea |
| :--- | :--- |
| `NaiveParser` | `mmap` the file and walk the length-prefixed messages in a single thread |
| `MTParser` | One producer frames the messages and shards them by stock to 4 worker threads via lock-free SPSC ring buffers, with batched pushes |

## Scope

This project focuses on **throughput of the I/O and dispatch path**, not on full protocol decoding.

- **Done:** message framing (length prefix, message type), zero-copy access to payloads through `mmap`, sharding by instrument, lock-free hand-off to workers, benchmarking harness.
- **Not done (yet):** decoding message fields (Add Order, Trade, ...) and maintaining an order book. In both variants the "consumer" work is a message counter, so the numbers below measure the framing and hand-off cost, not realistic downstream processing.

## Design

```
                       ┌──────────────┐   SPSC ring   ┌──────────┐
 mmap'd ITCH file ───▶ │   Producer   │ ────────────▶│ Worker 0 │
 (read-only)           │ frame + shard│   SPSC ring   ├──────────┤
                       │ + batch (32) │ ────────────▶ │ Worker 1 │
                       └──────────────┘      ...      │   ...    │
                                                      └──────────┘
```

- **Zero-copy input:** the file is mapped read-only (wrapped in a small RAII class, `MappedFile`, so the mapping is never leaked); messages are referenced by pointer, never copied out of the mapping.
- **Sharding:** the producer routes each message by the low byte of its *Stock Locate* field (`% 4`), so all messages of one instrument go to the same worker and per-instrument ordering is preserved.
- **SPSC ring buffer (`ringbuffer.hpp`):** power-of-two capacity, producer and consumer indices on separate cache lines (`alignas(64)`) to avoid false sharing, `acquire`/`release` ordering only (no sequentially consistent atomics, no locks).
- **Batching:** the producer collects 32 messages per worker in a local array and publishes them with a single `release` store, which reduces atomic traffic on the shared indices.
- **Interface:** `InterfaceParser` (`Parser.hpp`) lets `main.cpp` select the implementation at runtime (`-v naive|mt`).

## Benchmark

**Setup**

| | |
| :--- | :--- |
| Dataset | `01302020.NASDAQ_ITCH50`, 12,352 MiB (≈ 12.1 GiB), 423,285,709 messages |
| CPU | AMD EPYC 9554P (64 cores), university Linux server |
| Toolchain | GCC 13.3.0, `-O3`, CMake 3.28.3 |
| Method | 1 warm-up run + 10 measured runs per variant, median (min–max) reported; shared server |
| Page cache | Warm: the warm-up run loads the file into RAM, so disk speed is **not** measured |

**Results**

| Variant | Median time | Throughput | Messages/s |
| :--- | ---: | ---: | ---: |
| `NaiveParser` (1 thread) | 1.79 s (1.78–1.80) | 6,896 MiB/s | ≈ 236 M |
| `MTParser` (1 producer + 4 workers) | 5.67 s (4.94–5.79) | 2,179 MiB/s | ≈ 75 M |

Throughput is computed as file size / wall time, with MiB = 1024².

## Analysis: why is the multi-threaded version slower?

The naive scan is already close to memory-bound: ITCH messages are variable-length, and each message's start is only known after reading the previous one's length, so **framing is inherently sequential**. In `MTParser` this sequential walk still exists, it just lives in the producer thread. The producer therefore performs the same work as `NaiveParser` *plus* copying message descriptors into batches and publishing them to the ring buffers, while the workers have almost nothing to do (increment a counter). The result is a pipeline limited by its single producer (Amdahl's law), with extra overhead on top.

Parallelism can only help when the per-message work in the consumers is substantially more expensive than the framing step, for example decoding fields and updating a limit order book per instrument. That is the natural next step for this project.

## Limitations & Future Work

- Add real message decoding and a per-instrument order book as consumer workload, then re-run the comparison to find the break-even point.
- Input is assumed to be well-formed; a truncated file could cause an out-of-bounds read.
- Thread pinning, `madvise(MADV_SEQUENTIAL)` / huge pages, and hardware counters (`perf stat`) to separate cache, page-fault and synchronization effects.
- Sharding by `% 4` of one byte can produce uneven load; a better hash or dynamic balancing could be evaluated.
- Unit tests for the ring buffer (single-producer/single-consumer stress test, ideally under ThreadSanitizer).

## Build & Run

Requirements: Linux (POSIX `mmap`), x86-64, GCC ≥ 13, CMake ≥ 3.28.

```bash
git clone https://github.com/Lukas-Kroll/itch-parser.git
cd itch-parser

cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build --parallel

./build/ITCH_Parser -v naive -f /path/to/file.NASDAQ_ITCH50
./build/ITCH_Parser -v mt    -f /path/to/file.NASDAQ_ITCH50
```

| Flag | Meaning | Default |
| :--- | :--- | :--- |
| `-v`, `--version` | `naive` or `mt` | `naive` |
| `-f`, `--file` | path to the ITCH 5.0 file | `01302020.NASDAQ_ITCH50` |

**Data:** the dataset is not included because of its size. Sample ITCH 5.0 files are published by Nasdaq (e.g. `01302020.NASDAQ_ITCH50.gz`); decompress before use.

## Project Structure

| File | Purpose |
| :--- | :--- |
| `Parser.hpp` | `InterfaceParser` base class |
| `NaiveParser.{hpp,cpp}` | Single-threaded `mmap` scan |
| `MTParser.{hpp,cpp}` | Producer / sharded worker pipeline |
| `ringbuffer.hpp` | Lock-free SPSC ring buffer (single and batch operations) |
| `messages.h`, `message_information.h` | ITCH message definitions and the lightweight message view |
| `MappedFile.hpp` | RAII wrapper around `open`/`mmap`/`munmap`; the mapping is released automatically, also when an exception is thrown |
| `main.cpp` | CLI and benchmark harness |
