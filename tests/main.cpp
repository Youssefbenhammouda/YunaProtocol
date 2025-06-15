#include <iostream>
#include <thread>

#include "WindowsTransport.h"
#include "YunaNode.h"
//
// Created by youss on 6/14/2025.
//
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    auto transport = std::make_unique<YunaProtocol::WindowsTransport>(42069);


    transport->initialize();

    YunaProtocol::YunaNode node1(2);

    node1.addTransport(std::move(transport));

    node1.registerDataCallback("test_channel", [](YunaProtocol::Packet packet) {

        std::vector<uint8_t> data;
        data.reserve(packet.payload.size());
        data = std::move(packet.payload);
        std::cout << "Node 1 received data on channel: " << packet.header.channel << std::endl;
    });



        while (node1.listConnectedClients().empty()) {
            node1.loop();

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid busy waiting
        }


        //Test send messages
        std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
        node1.sendData(payload, "test_channel");

        // Run the loop to process incoming data
    while (1) {
        node1.loop();





    }

}
