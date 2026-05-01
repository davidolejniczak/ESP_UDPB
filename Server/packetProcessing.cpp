#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <thread>
#include <vector>

#define STARTNUM 1

inline uint32_t currentHostPacketId = STARTNUM;

inline uint32_t nextHostPacketId() {
    return currentHostPacketId++;
}

class UdpPacket {
public:
    std::vector<uint8_t> rawPayload;
    uint32_t hostPacketId;
    size_t packetSize;
    uint64_t hostReceivedTimestampNS;
    uint32_t ESPpacketId; 
    uint64_t ESPsentTimestampNS; 

    UdpPacket(): 
        hostPacketId(nextHostPacketId()),
        packetSize(0),
        hostReceivedTimestampNS(nowNS()),
        ESPpacketId(0),
        ESPsentTimestampNS(0) {}

    UdpPacket(const char* buffer, size_t bytesReceived): 
        rawPayload(buffer, buffer + bytesReceived),
        hostPacketId(nextHostPacketId()),
        packetSize(bytesReceived),
        hostReceivedTimestampNS(nowNS()),
        ESPpacketId(0), 
        ESPsentTimestampNS(0) {}

    static UdpPacket capture(const char* buffer, size_t bytesReceived) {
        return UdpPacket(buffer, bytesReceived);
    }

private:
    static uint64_t nowNS() {
        const auto now = std::chrono::steady_clock::now().time_since_epoch();
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()
        );
    }
};

// has its own thread 
class PacketProcessing {
public:
    inline static std::deque<UdpPacket> packets;

    static void init(std::atomic<bool>& processingRunning, std::atomic<bool>& serverRunning) {
        while (processingRunning.load() || serverRunning.load() || !packets.empty()) {
            std::optional<UdpPacket> packet = popPacket();

            if (!packet.has_value()) {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }
    }

    static std::optional<UdpPacket> popPacket() {
        if (packets.empty()) {
            return std::nullopt;
        }

        UdpPacket packet = packets.front();
        packets.pop_front();
        parsePacket(packet);

        return packet;
    }

    static bool parsePacket(UdpPacket& packet) {
        if (packet.rawPayload.size() < UDP_PAYLOAD_SIZE) {
            return false;
        }

        // little endian 
        packet.ESPpacketId =
            static_cast<uint32_t>(packet.rawPayload[0]) |
            (static_cast<uint32_t>(packet.rawPayload[1]) << 8) |
            (static_cast<uint32_t>(packet.rawPayload[2]) << 16) |
            (static_cast<uint32_t>(packet.rawPayload[3]) << 24);

        packet.ESPsentTimestampNS =
            static_cast<uint64_t>(packet.rawPayload[4]) |
            (static_cast<uint64_t>(packet.rawPayload[5]) << 8) |
            (static_cast<uint64_t>(packet.rawPayload[6]) << 16) |
            (static_cast<uint64_t>(packet.rawPayload[7]) << 24) |
            (static_cast<uint64_t>(packet.rawPayload[8]) << 32) |
            (static_cast<uint64_t>(packet.rawPayload[9]) << 40) |
            (static_cast<uint64_t>(packet.rawPayload[10]) << 48) |
            (static_cast<uint64_t>(packet.rawPayload[11]) << 56);

        return true;
    }

private:
    static constexpr size_t UDP_PAYLOAD_SIZE = sizeof(uint32_t) + sizeof(uint64_t);
};


// has its own thread so it can write to a file
// class PacketLoggin {
// public: 


// private:


// };
