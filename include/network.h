#ifndef NETWORK_H
#define NETWORK_H

#include <SFML/Network.hpp>
#include <optional>
#include <vector>
#include <mutex>

#include "sfml.h"

enum connect_status {
    // Universal error
    SUCCESS,
    SOCKET_CREATE_FAILED,
    SOCKET_CLOSE_ERROR,
    MESSENGE_SEND_ERROR,
    MESSENGE_RECV_ERROR,
    GAME_DATA_UPDATE_ERROR,
    GAME_INTERACTION_ERROR,

    // host connect error: 1XX
    HOST_ERROR_CODE_SERIES = 100,
    BIND_FAILED,
    LISTEN_ERROR,

    // client connect error: 2XX
    CLIENT_ERROR_CODE_SERIES = 200,
    CONNECT_FAILED,
    SERVER_NOT_RESPONDING
};

struct connect_data {
    sf::UdpSocket socket;
    sf::SocketSelector selector;
    std::optional<sf::IpAddress> server_ip;
    unsigned seed;
    unsigned short port = 6969;
};

class Network : public SFML {
public:
    Network(int width, int height, int mines, connect_data* data)
        : SFML(width, height, mines) {
        this->data = data;
    };
    ~Network();

    int play_multi(sf::IpAddress& ip, unsigned seed);
    int play_multi(std::vector<std::optional<sf::IpAddress>>& clients, unsigned seed);
    int recv_data();
    int recv_data(std::vector<sf::IpAddress>& clients);

protected:
    std::mutex mtx;
    connect_data* data;

    int generate_mines(unsigned seed);
    int send_data(sf::IpAddress& ip, sf::Packet& packet);
};

#endif