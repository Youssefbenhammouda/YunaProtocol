//
// Created by youss on 6/13/2025.
//

#ifndef TRANSPORT_H
#define TRANSPORT_H
#include <functional>

#include "Packet.h"
namespace YunaProtocol {
    class YunaTransport {


    public:
        using DataReceivedCallback = std::function<void(const Packet& packet)>;
        DataReceivedCallback callback;
        uint32_t clientID = 0;
        // Virtual destructor to ensure proper cleanup of derived classes.
        virtual ~YunaTransport() = default;

        /**
         * @brief Set the client ID for this transport layer.
         * @param clientId The unique identifier for this transport layer.

         */
        void setClientId(uint32_t clientId) {
            clientID = clientId;
        }


        /**
         * @brief Initializes the transport layer.

         */
        virtual void initialize() = 0;

        /**
         * @brief Sends data to a specific destination.
         * @param packet The data packet to send.
         * @return True if the data was sent successfully, false otherwise.
         */
        virtual bool send(const Packet& packet) = 0;


        /**
         * @brief Registers a callback to be invoked when data is received.
         * @param callback The function to call when data is received.
         */
        void registerDataReceivedCallback(const DataReceivedCallback& callback) ;

        /**
         * @brief Main loop to receive data then call callback.

         */
        virtual void loop() = 0;

        /**
         * @brief Broadcasts data to all devices on the network.
         * @param packet The data packet to broadcast.

         * @return True if the broadcast was successful, false otherwise.
         */
        virtual bool broadcast(const Packet& packet) = 0;

        /**
            * @brief List All clients connected to the transport layer.
        */
        virtual std::vector<uint32_t> listConnectedClients() = 0;

    };
}


#endif //TRANSPORT_H
