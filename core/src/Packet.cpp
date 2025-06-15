//
// Created by youss on 6/13/2025.
//

#include "Packet.h"
#include <cstring>
#include <cstddef> // Required for offsetof

using namespace YunaProtocol;

bool Packet::serialize(std::vector<uint8_t>& buffer) const {
    size_t totalSize = sizeof(PacketHeader) + payload.size();
    try {
        buffer.resize(totalSize);
    } catch (...) {
        return false; // Not enough memory
    }

    char* ptr = reinterpret_cast<char*>(buffer.data());

    // This is robust. The compiler calculates the correct offsets automatically.
    std::memcpy(ptr + offsetof(PacketHeader, protocolVersion), &header.protocolVersion, sizeof(header.protocolVersion));
    std::memcpy(ptr + offsetof(PacketHeader, packetType),      &header.packetType,      sizeof(header.packetType));
    std::memcpy(ptr + offsetof(PacketHeader, sourceId),        &header.sourceId,        sizeof(header.sourceId));
    std::memcpy(ptr + offsetof(PacketHeader, payloadLength),   &header.payloadLength,   sizeof(header.payloadLength));
    std::memcpy(ptr + offsetof(PacketHeader, channel),         &header.channel,         sizeof(header.channel));
    std::memcpy(ptr + offsetof(PacketHeader, channelPassword), &header.channelPassword, sizeof(header.channelPassword));

    if (!payload.empty()) {
        std::memcpy(ptr + sizeof(PacketHeader), payload.data(), payload.size());
    }

    return true;
}

bool Packet::deserialize(const uint8_t* buffer, size_t size) {
    if (size < sizeof(PacketHeader)) {
        return false;
    }

    const char* ptr = reinterpret_cast<const char*>(buffer);

    // This version is safe from unaligned access AND easy to maintain.
    std::memcpy(&header.protocolVersion, ptr + offsetof(PacketHeader, protocolVersion), sizeof(header.protocolVersion));
    std::memcpy(&header.packetType,      ptr + offsetof(PacketHeader, packetType),      sizeof(header.packetType));
    std::memcpy(&header.sourceId,        ptr + offsetof(PacketHeader, sourceId),        sizeof(header.sourceId));
    std::memcpy(&header.payloadLength,   ptr + offsetof(PacketHeader, payloadLength),   sizeof(header.payloadLength));
    std::memcpy(&header.channel,         ptr + offsetof(PacketHeader, channel),         sizeof(header.channel));
    std::memcpy(&header.channelPassword, ptr + offsetof(PacketHeader, channelPassword), sizeof(header.channelPassword));

    // Now that the header is safely populated, check the payload.
    if (size - sizeof(PacketHeader) != header.payloadLength) {
        return false;
    }

    payload.clear();
    if (header.payloadLength > 0) {
        if (size >= sizeof(PacketHeader) + header.payloadLength) {
            payload.resize(header.payloadLength);
            std::memcpy(payload.data(), ptr + sizeof(PacketHeader), header.payloadLength);
        } else {
            return false; // Packet size mismatch
        }
    }

    return true;
}