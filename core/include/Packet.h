//
// Created by youss on 6/13/2025.
//

#ifndef PACKET_H
#define PACKET_H
#include <cstdint>
#include <string>
#include <vector>

namespace YunaProtocol {
    enum PacketType {
        DISCOVERY_PEER = 0x01, // Discovery packet to find peers
        DATA = 0x02, // Data packet for communication
        ACKNOWLEDGEMENT = 0x03, // Acknowledgement packet: for confirming receipt of data
        PING = 0x04, // Ping packet for latency checks
    };

#pragma pack(push, 1) // Ensure struct is packed without padding
    struct PacketHeader {
        uint8_t protocolVersion = 1;
        PacketType packetType = PING;
        uint32_t sourceId{};
        char channel[32]{};
        uint64_t channelPassword = 0;
        uint16_t payloadLength{};

    };
#pragma pack(pop)

    struct Packet {
        PacketHeader header;
        std::vector<uint8_t> payload;
        /**
    * @brief Serializes the entire packet (header + payload) into a byte buffer.
    * @param buffer The vector to store the serialized data.
    * @return True if serialization is successful.
    */

        bool serialize(std::vector<uint8_t> &buffer) const;

        /**
 * @brief Deserializes a byte buffer into a Packet object.
 * @param buffer The byte buffer to deserialize.
 * @param size The size of the buffer.
 * @return True if deserialization is successful.
 */
        bool deserialize(const uint8_t *buffer, size_t size);
    };
}

#endif //PACKET_H
