//
// WindowsTransport.h
//

#ifndef WINDOWS_TRANSPORT_H
#define WINDOWS_TRANSPORT_H

// --- System Includes ---
// The following headers are required for Windows Sockets (Winsock)
#define WINDOWS_DISCOVERY_INTERVAL 5000
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <vector>

// --- Project Includes ---
#include "Transport.h" // Base class interface
#include <chrono>

// --- Library Linking ---
// This pragma directive automatically links the required Ws2_32.lib for Winsock functions.
#pragma comment(lib, "Ws2_32.lib")

namespace YunaProtocol {

    /**
     * @class WindowsTransport
     * @brief An implementation of the YunaTransport interface for Windows using UDP.
     *
     * This class handles the low-level networking details for sending and receiving
     * YunaProtocol::Packet objects over a UDP socket. It supports unicast, broadcast,
     * and automatic discovery of clients.
     */
    class WindowsTransport : public YunaTransport {
    public:
        /**
         * @brief Constructs a WindowsTransport instance.
         * @param port The UDP port to listen on for incoming packets. Defaults to 42069.
         */
        explicit WindowsTransport(int port = 42069);

        /**
         * @brief Destructor.
         *
         * Ensures that the socket is closed and Winsock is cleaned up properly.
         */
        ~WindowsTransport() override;

        // --- Overridden Interface Methods ---

        /**
         * @brief Initializes the transport layer.
         *
         * This method performs the following steps:
         * 1. Initializes the Winsock library.
         * 2. Creates a UDP socket.
         * 3. Binds the socket to the specified port and any available IP address.
         * 4. Enables broadcasting on the socket.
         * 5. Sets the socket to non-blocking mode to prevent the loop() from halting execution.
         */
        bool initialize() override;

        /**
         * @brief Sends a packet to a specific destination.
         *
         * @note This implementation assumes the packet's `sourceId` field is used
         * to specify the **destination client ID**. The transport layer sends the
         * packet to the IP address and port that were last seen associated with
         * that client ID.
         *
         * @param packet The packet to send. The header's `sourceId` must match a known client.
         * @return True if the packet was sent successfully, false if the client is unknown or an error occurred.
         */
        bool send(const Packet& packet) override;

        /**
         * @brief Receives incoming packets and invokes the registered callback.
         *
         * This method should be called repeatedly in a loop. It checks for incoming
         * data on the UDP socket. If a packet is received, it is deserialized, the
         * sender's address is recorded, and the `DataReceivedCallback` is triggered.
         * Since the socket is non-blocking, this call returns immediately if no data is available.
         */
        void loop() override;

        /**
         * @brief Broadcasts a packet to all devices on the local network.
         *
         * @param packet The packet to broadcast.
         * @return True if the broadcast was sent successfully, false otherwise.
         */
        bool broadcast(const Packet& packet) override;

        /**
         * @brief Lists the unique IDs of all clients from which a packet has been received.
         * @return A vector of client source IDs.
         */
        std::vector<uint32_t> listConnectedClients() override;



        void set_broadcast_port(int port)  ;


        ;

    private:
        // --- Member Variables ---

        SOCKET listenSocket;                                  // The primary socket for all network operations.
        sockaddr_in serverAddr;                               // The local address this transport is bound to.
        int listeningPort;                                           // The port number for listening
        int broadcastPort;                                            // The port number for  broadcasting.
        std::map<uint32_t, sockaddr_in> clients;              // A map to store the addresses of known clients [ClientID -> Address].
        bool initialized;
        std::chrono::steady_clock::time_point lastDiscoveryBroadcast{};// Flag to track if initialize() has been called successfully.
    };

} // namespace YunaProtocol

#endif //WINDOWS_TRANSPORT_H
