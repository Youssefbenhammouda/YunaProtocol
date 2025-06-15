//
// ESP8266Transport.h
//

#ifndef ESP8266TRANSPORT_H
#define ESP8266TRANSPORT_H

#include "Transport.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <map>

namespace YunaProtocol {

    /**
     * @class ESP8266Transport
     * @brief An implementation of the YunaTransport interface for the ESP8266 platform using UDP.
     *
     * This class handles network communication over Wi-Fi, including broadcasting for
     * peer discovery and sending/receiving data packets.
     */
    class ESP8266Transport : public YunaTransport {
    private:
        WiFiUDP udp; // The underlying UDP client for the ESP8266
        int listeningPort;
        int broadcastPort;
        bool initialized;
        unsigned long lastDiscoveryBroadcast; // Timestamp of the last discovery broadcast

        // A map to store discovered clients, mapping their client ID to their IP address.
        std::map<uint32_t, IPAddress> clients;

    public:
        /**
         * @brief Constructs a new ESP8266Transport object.
         * @param port The port to listen on for incoming packets and to broadcast on.
         */
        explicit ESP8266Transport(int port);

        /**
         * @brief Destructor. Cleans up resources.
         */
        ~ESP8266Transport() override;

        // --- Interface Implementation ---

        /**
         * @brief Initializes the transport layer. Must be called before any other operation.
         * It starts listening for UDP packets on the specified port.
         */
        bool initialize() override;

        /**
         * @brief The main loop for the transport layer. Should be called repeatedly.
         * It handles receiving packets and broadcasting discovery messages.
         */
        void loop() override;

        /**
         * @brief Sends a packet to all clients on the network (broadcast).
         * @param packet The packet to send.
         * @return True if the packet was sent successfully, false otherwise.
         */
        bool send(const Packet& packet) override;

        /**
         * @brief Broadcasts a packet to all devices on the network.
         * @param packet The packet to broadcast.
         * @return True if the broadcast was successful, false otherwise.
         */
        bool broadcast(const Packet& packet) override;

        /**
         * @brief Retrieves a list of all discovered client IDs.
         * @return A vector containing the unique IDs of all connected clients.
         */
        std::vector<uint32_t> listConnectedClients() override;

        /**
         * @brief Sets the port to be used for broadcasting.
         * @param port The broadcast port number.
         */
        void set_broadcast_port(int port);
    };

} // namespace YunaProtocol

#endif // ESP8266TRANSPORT_H
