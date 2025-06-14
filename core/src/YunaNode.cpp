//
// Created by youss on 6/12/2025.
//

#include "YunaNode.h"

#include <cstring>
#include <iostream>
#include <memory>

uint32_t YunaProtocol::YunaNode::getNodeId() const {
    return this->id;
}

void YunaProtocol::YunaNode:: registerDataCallback(const char channel[32],DataReceivedCallback callback)  {

    dataCallbacks[std::string(channel)]= std::move(callback);

}

YunaProtocol::YunaNode::YunaNode(uint32_t nodeID): id(nodeID) {
    // Initialize the node with a unique ID
    // Additional initialization logic can be added here if needed

}

YunaProtocol::YunaNode::~YunaNode() = default;

void YunaProtocol::YunaNode::sendData(std::vector<uint8_t> payload, const char channel[32]) {
    Packet packet;
    packet.header.sourceId = this->id;
    std::strncpy(packet.header.channel, channel, sizeof(packet.header.channel) - 1);
    packet.header.channel[sizeof(packet.header.channel) - 1] = '\0'; // Ensure null termination
    packet.header.payloadLength = static_cast<uint16_t>(payload.size());
    packet.payload = std::move(payload);
    for (auto &transport : transports) {
        transport->send(packet);

    }


}

void YunaProtocol::YunaNode::addTransport(std::unique_ptr<YunaTransport> transport) {
    transport->setClientId(id);
    transport->registerDataReceivedCallback(    [this](const YunaProtocol::Packet& packet) {
        this->handleDataPacket(packet);
    });
    transports.push_back(std::move(transport));

}

void YunaProtocol::YunaNode::loop() const {
    for (auto &transport : transports) {
        transport->loop();
    }
}

void YunaProtocol::YunaNode::handleDataPacket(const Packet& packet) const {
    std::string channel(packet.header.channel);
    auto it = dataCallbacks.find(channel);
    if (it != dataCallbacks.end()) {
        it->second(packet); // Call the registered callback with the packet
    } else {
        std::cerr << "No callback registered for channel: " << channel << std::endl;
    }


}

std::vector<uint32_t>  YunaProtocol::YunaNode::listConnectedClients() {
    std::vector<uint32_t> connectedClients;
    for (const auto& transport : transports) {
        auto clients = transport->listConnectedClients();
        connectedClients.insert(connectedClients.end(), clients.begin(), clients.end());
    }
    return connectedClients;

}
