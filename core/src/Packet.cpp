// Corrected Packet.cpp

#include "Packet.h"
#include <cstring>

using namespace YunaProtocol;

// SERIALIZE: Member by member
bool Packet::serialize(std::vector<uint8_t>& buffer) const {
    size_t headerSize = sizeof(PacketHeader);
    size_t payloadSize = payload.size();
    size_t totalSize = headerSize + payloadSize;
    buffer.resize(totalSize);

    // Use a pointer to step through the buffer
    char* ptr = reinterpret_cast<char*>(buffer.data());

    // Serialize header field by field
    std::memcpy(ptr, &header.protocolVersion, sizeof(header.protocolVersion));
    ptr += sizeof(header.protocolVersion);

    std::memcpy(ptr, &header.packetType, sizeof(header.packetType));
    ptr += sizeof(header.packetType);

    std::memcpy(ptr, &header.sourceId, sizeof(header.sourceId));
    ptr += sizeof(header.sourceId);

    std::memcpy(ptr, &header.channel, sizeof(header.channel));
    ptr += sizeof(header.channel);

    std::memcpy(ptr, &header.channelPassword, sizeof(header.channelPassword));
    ptr += sizeof(header.channelPassword);

    // Important: Update payloadLength in the header before serializing it
    PacketHeader tempHeader = header;
    tempHeader.payloadLength = payloadSize;
    std::memcpy(ptr, &tempHeader.payloadLength, sizeof(tempHeader.payloadLength));
    ptr += sizeof(tempHeader.payloadLength);


    // Serialize payload
    if (payloadSize > 0) {
        std::memcpy(ptr, payload.data(), payloadSize);
    }

    return true;
}

// DESERIALIZE: Member by member
bool Packet::deserialize(const uint8_t* buffer, size_t size) {
    size_t headerSize = sizeof(PacketHeader);
    if (size < headerSize) {
        return false;
    }

    const char* ptr = reinterpret_cast<const char*>(buffer);

    // Deserialize header field by field
    std::memcpy(&header.protocolVersion, ptr, sizeof(header.protocolVersion));
    ptr += sizeof(header.protocolVersion);

    std::memcpy(&header.packetType, ptr, sizeof(header.packetType));
    ptr += sizeof(header.packetType);

    std::memcpy(&header.sourceId, ptr, sizeof(header.sourceId));
    ptr += sizeof(header.sourceId);

    std::memcpy(&header.channel, ptr, sizeof(header.channel));
    ptr += sizeof(header.channel);

    std::memcpy(&header.channelPassword, ptr, sizeof(header.channelPassword));
    ptr += sizeof(header.channelPassword);

    std::memcpy(&header.payloadLength, ptr, sizeof(header.payloadLength));
    ptr += sizeof(header.payloadLength);

    // Verify payload size
    if (size - headerSize < header.payloadLength) {
        return false;
    }

    // Deserialize payload
    payload.resize(header.payloadLength);
    if (header.payloadLength > 0) {
        std::memcpy(payload.data(), ptr, header.payloadLength);
    }

    return true;
}