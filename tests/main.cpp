#include <iostream>
#include <thread>

#include "WindowsTransport.h"
#include "YunaNode.h"
//
// Created by youss on 6/14/2025.
//
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    YunaProtocol::WindowsTransport transport1(42069);
    transport1.set_broadcast_port(42068);
    YunaProtocol::WindowsTransport transport2(42068);
    transport2.set_broadcast_port(42069);
    transport1.initialize();
    transport2.initialize();
    YunaProtocol::YunaNode node1(1);
    YunaProtocol::YunaNode node2(2);
    node1.addTransport(std::make_unique<YunaProtocol::WindowsTransport>(transport1));
    node2.addTransport(std::make_unique<YunaProtocol::WindowsTransport>(transport2));
    node1.registerDataCallback("test_channel", [](YunaProtocol::Packet packet) {

        std::vector<uint8_t> data;
        data.reserve(packet.payload.size());
        data = std::move(packet.payload);
        std::cout << "Node 1 received data on channel: " << packet.header.channel << std::endl;
    });
    node2.registerDataCallback("test_channel", [](YunaProtocol::Packet packet) {
        std::cout << "Node 2 received data on channel: " << packet.header.channel << std::endl;
    });


        while (node1.listConnectedClients().size() ==0 || node2.listConnectedClients().size() == 0) {
            node1.loop();
            node2.loop();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid busy waiting
        }


        //Test send messages
        std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
        node1.sendData(payload, "test_channel");
        node2.sendData(payload, "test_channel");
        // Run the loop to process incoming data
    while (1) {
        node1.loop();
        node2.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid busy waiting



    }

}
