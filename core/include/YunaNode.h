#ifndef YUNANODE_H
#define YUNANODE_H
#include <functional>
#include <memory>


#include "Packet.h"
#include "Transport.h"
namespace YunaProtocol {

    class YunaNode {



    protected:
        uint32_t id = 0;
        std::unordered_map <std::string, DataReceivedCallback> dataCallbacks;
        std::vector<std::unique_ptr<YunaTransport>> transports;



    public:

        explicit YunaNode(uint32_t nodeID) ;;
        ~YunaNode();

        /**
    * @brief Gets the unique identifier for this node.
    * @return The node's 32-bit ID.
    */
         uint32_t getNodeId() const;
        /**
     * @brief Registers the application callback for incoming DATA packets.
     * @param channel The channel name to register the callback for.
     * @param callback The function to execute when a DATA packet is received.
     */
         void registerDataCallback(const std::string& channel,DataReceivedCallback callback) ;

        /**
     * @brief Sends data to a known destination node.
     * @param payload The payload to send.
     * @param channel The channel to send the data on.
     */
         void sendData(std::vector<uint8_t> payload, const char channel[32]) ;

         void addTransport(std::unique_ptr<YunaTransport> transport) ;

        /**
         *@brief main loop to receive data and call the registered callbacks.
         */
            void loop() const;

        void handleDataPacket(const Packet& packet) const ;

         std::vector<uint32_t> listConnectedClients() ;

    };
}

#endif //YUNANODE_H
