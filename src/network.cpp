#include "network.h"

#include <iostream>
#include <random>
#include <vector>

#include "board.h"

Network::~Network() { data->socket.unbind(); }

int Network::generate_mines(unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dis(0, this->row * this->col - 1);
    for (int i = 0; i < this->n_mines; i++) {
        int rand = dis(gen);
        if (this->blocks[rand].value >= block::MINE) {
            i--;
            continue;
        }

        this->blocks[rand].value = 9;
        for (int j = -1; j <= 1; j++) {
            if (rand < row && j == -1) continue;
            if (rand >= row * (col - 1) && j == 1) continue;

            for (int k = -1; k <= 1; k++) {
                if (rand % row == 0 && k == -1) continue;
                if (rand % row == row - 1 && k == 1) continue;
                if (this->blocks[rand + j * row + k].value < block::MINE) {
                    this->blocks[rand + j * row + k].value++;
                }
            }
        }
    }

    while (true) {
        int rand = dis(gen);
        if (this->blocks[rand].value == block::EMPTY) {
            reveal(blocks[rand]);
            break;
        }
    }

    return 0;
}

int Network::send_data(sf::IpAddress& ip, sf::Packet& packet) {
    if (data->socket.send(packet, ip, data->port) != sf::Socket::Done) {
        std::cerr << "Failed to send packet" << std::endl;
        return MESSENGE_SEND_ERROR;
    }
    return SUCCESS;
}

sf::Packet& operator>>(sf::Packet& packet, const SFML::game_action& action) {
    return packet;
}

int Network::recv_data() {
    sf::Packet recv_packet;
    sf::IpAddress ip;
    while (window.isOpen() && status == PLAYING) {
        if (data->socket.receive(recv_packet, ip, data->port) !=
            sf::Socket::Done) {
            std::cerr << "Failed to receive packet" << std::endl;
            return MESSENGE_RECV_ERROR;
        }

        game_action action;
        int block;
        recv_packet >> action >> block;
        mtx.lock();
        if (action == REVEAL) {
            reveal(blocks[block]);
        } else if (action == FLAG) {
            flip_flag(blocks[block]);
        }
        mtx.unlock();
    }

    return SUCCESS;
}

int Network::recv_data(std::vector<sf::IpAddress>& clients) {
    sf::Packet recv_packet;
    sf::IpAddress ip;
    while (window.isOpen() && status == PLAYING) {
        if (data->socket.receive(recv_packet, ip, data->port) !=
            sf::Socket::Done) {
            std::cerr << "Failed to receive packet" << std::endl;
            return MESSENGE_RECV_ERROR;
        }

        game_action action;
        int block;
        recv_packet >> action >> block;
        mtx.lock();
        if (action == REVEAL) {
            reveal(blocks[block]);
        } else if (action == FLAG) {
            flip_flag(blocks[block]);
        }
        mtx.unlock();

        for (auto& client : clients) {
            if (client != ip) {
                send_data(client, recv_packet);
            }
        }
    }

    return SUCCESS;
}

int Network::play_multi(sf::IpAddress& ip, unsigned seed) {
    init_block();
    generate_mines(seed);
    while (window.isOpen() && status == PLAYING) {
        window.clear(sf::Color::Black);
        draw_board();
        draw_flag();
        draw_time();
        window.display();

        mtx.lock();
        auto input = mouse_input();
        mtx.unlock();

        if (input.second == -2) {
            return 0;
        }
        if (input.first != NONE) {
            sf::Packet packet;
            packet << input.first << input.second;
            if (send_data(ip, packet) != SUCCESS) {
                return GAME_INTERACTION_ERROR;
            }
        }
        check_win();
    }
    end_game();

    return 0;
}

int Network::play_multi(std::vector<sf::IpAddress>& clients, unsigned seed) {
    init_block();
    generate_mines(seed);
    while (window.isOpen() && status == PLAYING) {
        window.clear(sf::Color::Black);
        draw_board();
        draw_flag();
        draw_time();
        window.display();

        mtx.lock();
        auto input = mouse_input();
        mtx.unlock();

        if (input.second == -2) {
            return 0;
        }
        if (input.first != NONE) {
            sf::Packet packet;
            packet << input.first << input.second;
            for (auto& ip : clients) {
                if (send_data(ip, packet) != SUCCESS) {
                    return GAME_INTERACTION_ERROR;
                }
            }
        }
        check_win();
    }
    end_game();

    return 0;
}