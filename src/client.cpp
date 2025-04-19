#include "client.h"

#include <memory>
#include <iostream>
#include <thread>
#include <string>

#include <SFML/Network.hpp>

#include "network.h"

int client() {
    // 使用智慧指標實例化 connect_data
    auto connection = std::make_unique<connect_data>();
    sf::Packet packet;
    unsigned seed;

    if (connection->socket.bind(connection->port) != sf::Socket::Status::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }

    do {
        std::string ip;
        std::cout << "Enter server ip: ";
        std::cin >> ip;
        connection->server_ip = sf::IpAddress::resolve(ip);
    } while (!connection->server_ip.has_value());

    connection->selector.add(connection->socket);
    packet << "Minesweeper";
    if (connection->socket.send(packet,
                                connection->server_ip.value(),
                                connection->port) != sf::Socket::Status::Done) {
        std::cerr << "Failed to send packet" << std::endl;
        return MESSENGE_SEND_ERROR;
    }
    if (connection->selector.wait(sf::seconds(10))) {
        sf::Socket::Status status =
            connection->socket.receive(packet,
                                       connection->server_ip,
                                       connection->port);
        if (status == sf::Socket::Status::Done) {
            std::cout << "Connected to server!\n" << std::endl;
        } else {
            std::cerr << "Failed to receive packet" << std::endl;
            return MESSENGE_RECV_ERROR;
        }
    } else {
        std::cerr << "Server not responding" << std::endl;
        return SERVER_NOT_RESPONDING;
    }

    if (connection->socket.receive(packet,
                                    connection->server_ip,
                                    connection->port) != sf::Socket::Status::Done) {
        std::cerr << "Failed to receive packet" << std::endl;
        return MESSENGE_RECV_ERROR;
    }
    packet >> seed;

    // 釋放智慧指標，傳 raw pointer 給 Network
    connect_data* raw_conn = connection.release();
    Network game(30, 16, 99, raw_conn);
    std::thread(static_cast<int (Network::*)()>(&Network::recv_data), &game).detach();
    game.play_multi(raw_conn->server_ip.value(), seed);

    return 0;
}