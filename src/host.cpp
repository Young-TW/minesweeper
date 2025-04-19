#include "host.h"

#include <thread>
#include <iostream>
#include <random>

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "network.h"

int host() {
    // 使用智慧指標管理 connect_data
    auto connection = std::make_unique<connect_data>();
    connection->server_ip = sf::IpAddress::getLocalAddress();
    std::vector<std::optional<sf::IpAddress>> clients;

    if (connection->socket.bind(connection->port) != sf::Socket::Status::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }
    connection->selector.add(connection->socket);
    std::cout << "Server ip: " << connection->server_ip.value() << std::endl;

    std::cout << "Waiting for clients... (Press Enter to start the game)" << std::endl;
    int player_count = 0;
    while (connection->selector.wait()) {
        if (connection->selector.isReady(connection->socket)) {
            std::optional<sf::IpAddress> client = sf::IpAddress::Any;
            sf::Packet packet;
            if (connection->socket.receive(packet,
                                          client,
                                          connection->port) !=
                sf::Socket::Status::Done) {
                std::cerr << "Failed to receive packet" << std::endl;
                return MESSENGE_RECV_ERROR;
            }
            if (connection->socket.send(packet,
                                       client.value(),
                                       connection->port) !=
                sf::Socket::Status::Done) {
                std::cerr << "Failed to send packet" << std::endl;
                return MESSENGE_SEND_ERROR;
            }
            if (std::find(clients.begin(), clients.end(), client) ==
                clients.end()) {
                clients.push_back(client);
            }

            std::cout << "Client connected: " << client.value() << std::endl;
            player_count++;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (std::cin.get() == '\n' && player_count > 0) {
            break;
        }
    }

    sf::Packet seed;
    seed << std::random_device{}();
    for (auto& client : clients) {
        if (connection->socket.send(seed,
                                   client.value(),
                                   connection->port) != sf::Socket::Status::Done) {
            std::cerr << "Failed to send packet" << std::endl;
            return MESSENGE_SEND_ERROR;
        }
    }

    unsigned seed_value;
    seed >> seed_value;

    // 釋放 pointer
    connect_data* raw_conn = connection.release();
    Network game(30, 16, 99, raw_conn);

    // 準備純 IP List 供 recv_data
    std::vector<sf::IpAddress> ipClients;
    for (auto& opt : clients) {
        if (opt) ipClients.push_back(opt.value());
    }
    std::thread(static_cast<int (Network::*)(std::vector<sf::IpAddress>&)>(&Network::recv_data), &game, std::ref(ipClients)).detach();
    game.play_multi(clients, seed_value);

    return 0;
}
