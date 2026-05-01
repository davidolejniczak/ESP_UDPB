# ESP32 UDP Benchmarking Engine

A UDP benchmarking project built around an ESP32-WROOM module and a host-side C++ receiver. The ESP32 sends lightweight UDP packets over WiFi, while the host receives, timestamps, queues, and processes packets for throughput, latency, packet loss, and jitter analysis.

## Current Packet Format

The ESP32 UDP payload is currently 12 bytes:

```
uint32_t espPacketId       // 4 bytes, little-endian
uint64_t espTimestampNS    // 8 bytes, little-endian
```

The host also assigns each received packet a local `hostPacketId`, records the received packet size, and records a host-side receive timestamp.

## Host Server Flow

The C++ host server is split into two main paths:

```
Core server thread:
    capture raw packet bytes
    push packet into queue

Packet processing thread:
    pop packet from queue
    parse packet info 

File logging thread: 
    writes packet info to file
    
```

This keeps parsing off the UDP receive hot path.

## Build Host Server

From the `Server` directory:

```bash
clang++ -std=c++17 main.cpp -o main
```

Run:

```bash
./main
```

## Folder contents

```
ESP32/
    CMakeLists.txt
    main/
        CMakeLists.txt
        hello_world_main.c

Server/
    main.cpp
    coreserver.cpp
    packetProcessing.cpp
```

## ESP32 Hardware Capabilities 

ESP: ESP32-D0WD-V3 revision v3.1  
CPU frequency: 160 MHz  
Flash: 4 MB  
PSRAM: None  
Wireless: WiFi + Bluetooth
