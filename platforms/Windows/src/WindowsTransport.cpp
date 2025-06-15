//
// WindowsTransport.cpp
//

#include "WindowsTransport.h"
#include <iostream> // For error logging


namespace YunaProtocol {

    // --- Constructor & Destructor ---

    WindowsTransport::WindowsTransport(int port)
        : listenSocket(INVALID_SOCKET), listeningPort(port), broadcastPort(port), initialized(false),serverAddr{} {
        // The constructor initializes member variables.
        // `INVALID_SOCKET` is the default state for a socket before it's created.

    }

    WindowsTransport::~WindowsTransport() {
        // Cleanup is crucial for network applications.
        if (listenSocket != INVALID_SOCKET) {
            closesocket(listenSocket); // Close the socket resource.
        }
        if (initialized) {
            WSACleanup(); // Terminate the use of the Winsock DLL.
        }
    }

    // --- Interface Implementation ---

    bool WindowsTransport::initialize() {
        // 1. Initialize Winsock
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed with error: " << result << std::endl;
            return false;
        }

        // 2. Create a UDP socket
        listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }

        // 3. Bind the socket to a local address and port
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(this->listeningPort);
        serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on any available network interface

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        // 4. Enable broadcasting
        char broadcastOption = '1';
        if (setsockopt(listenSocket, SOL_SOCKET, SO_BROADCAST, &broadcastOption, sizeof(broadcastOption)) == SOCKET_ERROR) {
            std::cerr << "setsockopt SO_BROADCAST failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        // 5. Set the socket to non-blocking mode
        u_long mode = 1; // 1 for non-blocking, 0 for blocking
        if (ioctlsocket(listenSocket, FIONBIO, &mode) == SOCKET_ERROR) {
            std::cerr << "ioctlsocket FIONBIO failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        initialized = true;
        std::cout << "WindowsTransport initialized successfully on port " << this->listeningPort << "." << std::endl;
        return true;
    }

    void WindowsTransport::loop() {
        if (!initialized) return;
        // Broadcast Discovery Peer Packet
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastDiscoveryBroadcast);

        if (elapsed.count() > WINDOWS_DISCOVERY_INTERVAL) { // Broadcast only once per second



            lastDiscoveryBroadcast = now;
            Packet discoveryPacket;
            discoveryPacket.header.protocolVersion = 1;
            discoveryPacket.header.packetType = DISCOVERY_PEER;
            discoveryPacket.header.sourceId = clientID; // Use 0 or a specific ID for discovery
            discoveryPacket.header.payloadLength = 0; // No payload for discovery
            discoveryPacket.header.channelPassword = 0; // No password for discovery

            // Broadcast the discovery packet to find peers
            if (!broadcast(discoveryPacket)) {
                std::cerr << "Failed to broadcast discovery packet." << std::endl;
            } else {
                //std::cout << "Discovery packet broadcasted successfully." << std::endl;
            }


        }










        // Prepare to receive data from the socket.
        const int bufferSize = 4096; // A reasonable buffer size for UDP packets
        char buffer[bufferSize];
        sockaddr_in senderAddr{};
        int senderAddrSize = sizeof(senderAddr);

        // Attempt to receive data. Since the socket is non-blocking, this returns immediately.
        int bytesReceived = recvfrom(listenSocket, buffer, bufferSize, 0, (sockaddr*)&senderAddr, &senderAddrSize);

        if (bytesReceived > 0) {
            // Data was received, now process it.
            Packet receivedPacket;
            if (receivedPacket.deserialize(reinterpret_cast<uint8_t*>(buffer), bytesReceived)) {
                if (receivedPacket.header.sourceId == clientID){return;}
                //uint32_t clientId = receivedPacket.header.sourceId;
                if (receivedPacket.header.packetType == DISCOVERY_PEER || clients.find(receivedPacket.header.sourceId) == clients.end() ) { // or sender is not in clients
                    // Handle discovery packet logic here if needed.
                    // /std::cout << "Discovery packet received from client ID: " << receivedPacket.header.sourceId << std::endl;
                    // CHeck if not in clients map then add it
                    if (clients.find(receivedPacket.header.sourceId) == clients.end()) {
                        char ipStr[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
                        std::cout << "New client discovered with ID, addr: " << receivedPacket.header.sourceId << " " << std::string(ipStr) + ":" + std::to_string(ntohs(senderAddr.sin_port))<< std::endl;
                        // Add the new client to the clients map.
                        clients[receivedPacket.header.sourceId] = senderAddr;
                    }
                }
                if (receivedPacket.header.packetType != DISCOVERY_PEER) {





                    // If a callback is registered, invoke it with the received packet.
                    if (callback) {
                        callback(receivedPacket);
                    }
                }
            } else {
                 std::cerr << "Failed to deserialize packet of size " << bytesReceived << std::endl;
            }
        } else if (bytesReceived == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            // WSAEWOULDBLOCK is expected in non-blocking mode and means no data is available.
            // We can safely ignore it.
            if (errorCode != WSAEWOULDBLOCK) {
                std::cerr << "recvfrom failed with error: " << errorCode << std::endl;
            }
        }
    }

    bool WindowsTransport::send(const Packet& packet) {
        if (!initialized) return false;


        // Serialize the packet into a buffer.
        std::vector<uint8_t> buffer;
        if (!packet.serialize(buffer)) {
            std::cerr << "Failed to serialize packet for sending." << std::endl;
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
            // client_pair.second is the client's address (sockaddr_in)
            const sockaddr_in& clientAddr = client_pair.second;

            int bytesSent = sendto(
                listenSocket,
                (const char*)buffer.data(),
                static_cast<int>(buffer.size()),
                0,
                (const sockaddr*)&clientAddr,
                sizeof(clientAddr)
            );

            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "sendto failed for client " << client_pair.first
                          << " with error: " << WSAGetLastError() << std::endl;
                // You might want to return false here or continue to send to other clients
            }
        }









        return true;


    }

    bool WindowsTransport::broadcast(const Packet& packet) {
        if (!initialized) return false;

        // Serialize the packet into a buffer.
        std::vector<uint8_t> buffer;
        if (!packet.serialize(buffer)) {
            std::cerr << "Failed to serialize packet for broadcast." << std::endl;
            return false;
        }

        // Create the broadcast address structure.
        sockaddr_in broadcastAddr{};
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(this->broadcastPort); // Broadcast on the same port we listen on.
        broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

        // Send the buffer.
        int bytesSent = sendto(listenSocket, (const char*)buffer.data(), static_cast<int>(buffer.size()), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "broadcast sendto failed with error: " << WSAGetLastError() << std::endl;
            return false;
        }

        return bytesSent == buffer.size();
    }

    std::vector<uint32_t> WindowsTransport::listConnectedClients() {
        std::vector<uint32_t> clientIds;
        // Reserve space for efficiency.
        clientIds.reserve(clients.size());

        // Iterate through the map and extract the keys (client IDs).
        for (const auto&[fst, snd] : clients) {
            clientIds.push_back(fst);
        }
        return clientIds;
    }


    void WindowsTransport::set_broadcast_port(const int port)  {
        this->broadcastPort = port;
    }



} // namespace YunaProtocol



