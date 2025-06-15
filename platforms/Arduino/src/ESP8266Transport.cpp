//
// ESP8266Transport.cpp
//

#include "ESP8266Transport.h"

// Define a constant for the discovery broadcast interval (in milliseconds)
#define DISCOVERY_INTERVAL 5000

namespace YunaProtocol {

    // --- Constructor & Destructor ---

    ESP8266Transport::ESP8266Transport(int port)
        : listeningPort(port), broadcastPort(port), initialized(false), lastDiscoveryBroadcast(0) {
        // The constructor initializes member variables.
    }

    ESP8266Transport::~ESP8266Transport() {
        // Stop the UDP client if it was running.
        if (initialized) {
            udp.stop();
        }
    }

    // --- Interface Implementation ---

    bool ESP8266Transport::initialize() {
        // 1. Check if the ESP8266 is connected to Wi-Fi.
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Error: Wi-Fi not connected. Cannot initialize ESP8266Transport.");
            return false;
        }

        // 2. Start the UDP client on the specified listening port.
        if (!udp.begin(listeningPort)) {
            Serial.printf("Error: Failed to start UDP listener on port %d\n", listeningPort);
            return false;
        }

        initialized = true;
        Serial.printf("ESP8266Transport initialized successfully on port %d.\n", listeningPort);
        return true;
    }

    void ESP8266Transport::loop() {
        if (!initialized) return;

        // 1. Periodically broadcast a discovery packet to find other peers.
        if (millis() - lastDiscoveryBroadcast > DISCOVERY_INTERVAL) {
            lastDiscoveryBroadcast = millis(); // Reset the timer

            Packet discoveryPacket;
            discoveryPacket.header.protocolVersion = 1;
            discoveryPacket.header.packetType = DISCOVERY_PEER;
            discoveryPacket.header.sourceId = clientID;
            discoveryPacket.header.payloadLength = 0;
            discoveryPacket.header.channelPassword = 0;

            if (!broadcast(discoveryPacket)) {
                Serial.println("Error: Failed to broadcast discovery packet.");
            }
        }

        // 2. Check for and process incoming UDP packets.
        int packetSize = udp.parsePacket();
        if (packetSize > 0) {
            // A packet has been received.
            std::vector<uint8_t> buffer(packetSize);
            int bytesRead = udp.read(buffer.data(), packetSize);

            if (bytesRead > 0) {
                Packet receivedPacket;
                if (receivedPacket.deserialize(buffer.data(), bytesRead)) {
                    // Ignore packets sent by ourselves.
                    if (receivedPacket.header.sourceId == clientID) {
                        return;
                    }

                    // Handle peer discovery and client list management.
                    if (receivedPacket.header.packetType == DISCOVERY_PEER || clients.find(receivedPacket.header.sourceId) == clients.end()) {
                        IPAddress remoteIp = udp.remoteIP();
                        if (clients.find(receivedPacket.header.sourceId) == clients.end()) {
                            Serial.printf("New client discovered with ID: %u at %s\n", receivedPacket.header.sourceId, remoteIp.toString().c_str());
                            clients.emplace(receivedPacket.header.sourceId, remoteIp);
                        }
                    }

                    // For any packet that isn't for discovery, pass it to the callback.
                    if (receivedPacket.header.packetType != DISCOVERY_PEER) {
                        if (callback) {
                            callback(receivedPacket);
                        }
                    }
                } else {
                    Serial.printf("Error: Failed to deserialize packet of size %d\n", bytesRead);
                }
            }
        }
    }

    bool ESP8266Transport::send(const Packet& packet) {
        if (!initialized) return false;


        // Serialize the packet into a buffer.
        std::vector<uint8_t> buffer;
        if (!packet.serialize(buffer)) {
            Serial.println("Failed to serialize packet for sending.");
            return false;
        }

        if (clients.empty()) {
            // Optional: Log if there are no clients to send to.
            // std::cout << "No clients connected, nothing to send." << std::endl;
            return true; // Return true as there was no error.
        }
        // Send the packet to all clients in the map, sourceID is the node id not the destination id.
        for (const auto& client_pair : clients) {
            // client_pair.first is the client's node ID (uint32_t)
            // client_pair.second is the client's IP address (IPAddress)
            const IPAddress& clientAddr = client_pair.second;

            udp.beginPacket(clientAddr, broadcastPort);
            udp.write(buffer.data(), buffer.size());
            if (!udp.endPacket()) {
                Serial.printf("Failed to send packet to client %u at %s\n", client_pair.first, clientAddr.toString().c_str());
            }
        }








        return true;
    }

    bool ESP8266Transport::broadcast(const Packet& packet) {
        if (!initialized) return false;

        // Serialize the packet into a buffer.
        std::vector<uint8_t> buffer;
        if (!packet.serialize(buffer)) {
            Serial.println("Error: Failed to serialize packet for broadcast.");
            return false;
        }

        // The broadcast IP for a typical home network is 255.255.255.255.
        IPAddress broadcastIp(255, 255, 255, 255);

        // Send the packet.
        if (!udp.beginPacket(broadcastIp, broadcastPort)) {
            Serial.println("Error: udp.beginPacket() failed.");
            return false;
        }
        udp.write(buffer.data(), buffer.size());
        if (!udp.endPacket()) {
            Serial.println("Error: udp.endPacket() failed to send.");
            return false;
        }

        return true;
    }

    std::vector<uint32_t> ESP8266Transport::listConnectedClients() {
        std::vector<uint32_t> clientIds;
        clientIds.reserve(clients.size());

        for (const auto& pair : clients) {
            clientIds.push_back(pair.first);
        }
        return clientIds;
    }

    void ESP8266Transport::set_broadcast_port(int port) {
        this->broadcastPort = port;
    }

} // namespace YunaProtocol
