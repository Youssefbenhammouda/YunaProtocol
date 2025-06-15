//
// Created by youss on 6/13/2025.
//

#include "Packet.h"

#include <cstring>
using namespace YunaProtocol;
bool Packet::serialize(std::vector<uint8_t>& buffer) const {
    size_t totalSize = sizeof(PacketHeader) + payload.size();
    try {
        buffer.resize(totalSize);
    } catch (...) {
        return false; // Not enough memory
    }

    char* ptr = reinterpret_cast<char*>(buffer.data());

    // Safely copy from our aligned struct members to the unaligned buffer representation.
    // This is the portable way to handle packed structs without causing hardware exceptions.
    std::memcpy(ptr,      &header.protocolVersion, sizeof(header.protocolVersion));
    std::memcpy(ptr + 1,  &header.packetType,      sizeof(header.packetType));
    std::memcpy(ptr + 2,  &header.sourceId,        sizeof(header.sourceId));
    std::memcpy(ptr + 6,  &header.payloadLength,   sizeof(header.payloadLength));
    std::memcpy(ptr + 10, &header.channel,         sizeof(header.channel));
    std::memcpy(ptr + 42, &header.channelPassword, sizeof(header.channelPassword));

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

    // Safely copy each member from the potentially unaligned network buffer
    // into our struct's members. This avoids direct unaligned access by the CPU.
    std::memcpy(&header.protocolVersion, ptr,      sizeof(header.protocolVersion));
    std::memcpy(&header.packetType,      ptr + 1,  sizeof(header.packetType));
    std::memcpy(&header.sourceId,        ptr + 2,  sizeof(header.sourceId));
    std::memcpy(&header.payloadLength,   ptr + 6,  sizeof(header.payloadLength));
    std::memcpy(&header.channel,         ptr + 10, sizeof(header.channel));
    std::memcpy(&header.channelPassword, ptr + 42, sizeof(header.channelPassword));

    // Now that the header is safely populated, check the payload.
    if (size - sizeof(PacketHeader) != header.payloadLength) {
        return false;
    }

    payload.clear();
    if (header.payloadLength > 0) {
        // Ensure buffer is large enough before resizing and copying
        if (size >= sizeof(PacketHeader) + header.payloadLength) {
            payload.resize(header.payloadLength);
            std::memcpy(payload.data(), ptr + sizeof(PacketHeader), header.payloadLength);
        } else {
            return false; // Packet size mismatch
        }
    }

    return true;
}
