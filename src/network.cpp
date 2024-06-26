#include "network.h"
#include "board.h"

#include <iostream>
#include <optional>
#include <unordered_set>

int Network::client() {
    sf::UdpSocket socket;
    std::optional<sf::IpAddress> ip;
    sf::Packet packet;

    if (socket.bind(this->port) != sf::Socket::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }

    do {
        std::cout << "Enter server ip: ";
        std::cin >> ip.value();
    } while (ip.has_value());

    packet << "Minesweeper";
    if (socket.send(packet, ip.value(), this->port) != sf::Socket::Done) {
        std::cerr << "Failed to send packet" << std::endl;
        return MESSENGE_SEND_ERROR;
    }
    
    return 0;
}

int Network::host() {
    sf::UdpSocket socket;
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    std::unordered_set<sf::IpAddress> clients;
    sf::SocketSelector selector;
    unsigned int max_clients = 0;

    if (socket.bind(this->port) != sf::Socket::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }
    selector.add(socket);
    std::cout << "Server ip: " << ip << std::endl;

    do {
        std::cout << "Enter max clients(1 ~ 10):";
        std::cin >> max_clients;
    } while (max_clients < 1 || max_clients > 10);
    
    std::cout << "Waiting for clients..." << std::endl;
    while (selector.wait()) {
        if (selector.isReady(socket)) {
            sf::IpAddress client;
            sf::Packet packet;
            if (socket.receive(packet, client, this->port) != sf::Socket::Done) {
                std::cerr << "Failed to receive packet" << std::endl;
                return MESSENGE_RECV_ERROR;
            }
            clients.insert(client);
            if (clients.size() == max_clients) {
                break;
            }
        }
    }
    

    return 0;
}
