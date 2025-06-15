//
// Created by youss on 6/13/2025.
//

#include "Packet.h"

#include <cstring>
using namespace YunaProtocol;
bool Packet::serialize(std::vector<uint8_t> &buffer) const {
    size_t totalSize = sizeof(PacketHeader)+ payload.size();
    try{buffer.resize(totalSize);}
    catch (...) {
        return false;
    }
    std::memcpy(buffer.data(), &header, sizeof(PacketHeader));
    std::memcpy(buffer.data()+sizeof(PacketHeader), payload.data(), payload.size());
    return true;
}

bool Packet::deserialize(const uint8_t *buffer, size_t size) {
    if (size < sizeof(PacketHeader)) {
        return false;

    }
    std::memcpy(&header,buffer,sizeof(PacketHeader));

    if (size-sizeof(PacketHeader) < header.payloadLength) {
        return false;

    }
    payload.resize(header.payloadLength);
    std::memcpy(payload.data(), buffer + sizeof(PacketHeader), header.payloadLength);
    return true;

}

