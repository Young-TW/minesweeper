#include "menu.hpp"

#include <functional>
#include <iostream>
#include <random>
#include <thread>

#include "network.h"
#include "sfml.hpp"

Menu::Menu() {
    window.create(sf::VideoMode(260, 320), "Minesweeper");
    icon.loadFromFile("../image/icon/menu.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    if (!font.loadFromFile("../font/Cubic_11_1.100_R.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    if (!texture.loadFromFile("../image/menu_button.png")) {
        std::cerr << "Failed to load difficulty.png" << std::endl;
    }

    title.setFont(font);
    title.setString("Minesweeper!");
    title.setCharacterSize(24);
    title.setPosition(60, 20);
    title.setFillColor(sf::Color::White);

    sprite.setTexture(texture);
    sprite.setScale(2, 2);

    difficulty_text.setFont(font);
    difficulty_text.setCharacterSize(16);
    difficulty_text.setFillColor(sf::Color::White);

    menu_text.setFont(font);
    menu_text.setCharacterSize(26);
    menu_text.setFillColor(sf::Color::White);
}

int Menu::draw_button(int button_index, bool is_pressed) {
    if (button_index != 3 &&
        (button_index < 0 || button_index >= mode[mode_index].size())) {
        return -1;
    }

    int j = button_index == 3 ? 8 : button_index + mode_index * 2;
    int move = is_pressed ? 3 : 0;
    sprite.setTextureRect(sf::IntRect(131 * is_pressed, j * 27, 130, 26));
    sprite.setPosition(0, 80 + button_index * 60);
    window.draw(sprite);

    if (mode_index == 2 && button_index != 3) {
        difficulty_text.setString(
            mode[2][button_index] + "\n" +
            std::to_string(difficulty[button_index][0]) + "x" +
            std::to_string(difficulty[button_index][1]) + " " +
            std::to_string(difficulty[button_index][2]) + " mines");
        difficulty_text.setPosition(65 + move, 82 + move + button_index * 60);
        window.draw(difficulty_text);
    } else {
        if (button_index == 3) {
            menu_text.setString(!mode_index ? "Exit" : "Back");
        } else {
            menu_text.setString(mode[mode_index][button_index]);
        }
        menu_text.setPosition(65 + move, 82 + move + button_index * 60);
        window.draw(menu_text);
    }

    if (is_pressed) {
        window.display();
        while (window.waitEvent(event)) {
            if (event.type == sf::Event::MouseButtonReleased) {
                draw_button(button_index);
                window.display();
                mode_select(button_index);
                return 0;
            }
        }
    }

    return 0;
}

int Menu::draw_menu() {
    window.clear(sf::Color::Black);
    window.draw(title);
    draw_button(3);
    for (int i = 0; i < mode[mode_index].size(); i++) {
        draw_button(i);
    }
    window.display();

    int input = get_input();
    if (input != -1) {
        draw_button(input, true);
    }

    return 0;
}

int Menu::mode_select(int input) {
    if (input == 3) {
        if (mode_index == 0) {
            window.close();
        } else if (mode_index == 3) {
            mode_index = 1;
        } else {
            mode_index = 0;
        }
        return 0;
    }

    switch (mode_index) {
        case 0:
            if (input == 0) {
                mode_index = 2;
            } else if (input == 1) {
                mode_index = 1;
            }
            break;
        case 1:
            if (input == 0) {
                host();
            } else if (input == 1) {
                client();
            }
            mode_index = 3;
            break;
        case 2:
            if (input >= 0 && input <= 2) {
                SFML game(difficulty[input][0], difficulty[input][1],
                          difficulty[input][2]);
                game.play_single();
                mode_index = 0;
            }
            break;
    }

    return 0;
}

int Menu::get_input() {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (mouse.y >= 80 && mouse.y <= 132) {
            return 0;
        } else if (mouse.y >= 140 && mouse.y <= 192) {
            return 1;
        } else if (mouse.y >= 200 && mouse.y <= 252) {
            return 2;
        } else if (mouse.y >= 260 && mouse.y <= 312) {
            return 3;
        }
    }
    return -1;
}

int Menu::run() {
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        draw_menu();
    }
    return 0;
}

int Menu::client() {
    connect_data data;
    sf::Packet packet;
    unsigned seed;

    if (data.socket.bind(data.port) != sf::Socket::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }

    do {
        std::string ip;
        std::cout << "Enter server ip: ";
        std::cin >> ip;
        data.server_ip = sf::IpAddress(ip);
    } while (!data.server_ip.has_value());

    data.selector.add(data.socket);
    packet << "Minesweeper";
    if (data.socket.send(packet, data.server_ip.value(), data.port) !=
        sf::Socket::Done) {
        std::cerr << "Failed to send packet" << std::endl;
        return MESSENGE_SEND_ERROR;
    }
    if (data.selector.wait(sf::seconds(10))) {
        sf::Socket::Status status =
            data.socket.receive(packet, data.server_ip.value(), data.port);
        if (status == sf::Socket::Done) {
            std::cout << "Connected to server!\n" << std::endl;
        } else {
            std::cerr << "Failed to receive packet" << std::endl;
            return MESSENGE_RECV_ERROR;
        }
    } else {
        std::cerr << "Server not responding" << std::endl;
        return SERVER_NOT_RESPONDING;
    }

    if (data.socket.receive(packet, data.server_ip.value(), data.port) !=
        sf::Socket::Done) {
        std::cerr << "Failed to receive packet" << std::endl;
        return MESSENGE_RECV_ERROR;
    }
    packet >> seed;
    Network game(30, 16, 99, &data);
    std::thread(static_cast<int (Network::*)()>(&Network::recv_data), &game)
        .detach();
    game.play_multi(data.server_ip.value(), seed);

    return 0;
}

int Menu::host() {
    connect_data data;
    data.server_ip = sf::IpAddress::getLocalAddress();
    std::vector<sf::IpAddress> clients;
    unsigned int max_clients = 0;

    if (data.socket.bind(data.port) != sf::Socket::Done) {
        std::cerr << "Failed to bind socket" << std::endl;
        return SOCKET_CREATE_FAILED;
    }
    data.selector.add(data.socket);
    std::cout << "Server ip: " << data.server_ip.value() << std::endl;

    do {
        std::cout << "Enter max clients(1 ~ 10):";
        std::cin >> max_clients;
    } while (max_clients < 1 || max_clients > 10);

    std::cout << "Waiting for clients..." << std::endl;
    while (data.selector.wait()) {
        if (data.selector.isReady(data.socket)) {
            sf::IpAddress client;
            sf::Packet packet;
            if (data.socket.receive(packet, client, data.port) !=
                sf::Socket::Done) {
                std::cerr << "Failed to receive packet" << std::endl;
                return MESSENGE_RECV_ERROR;
            }
            if (data.socket.send(packet, client, data.port) !=
                sf::Socket::Done) {
                std::cerr << "Failed to send packet" << std::endl;
                return MESSENGE_SEND_ERROR;
            }
            if (std::find(clients.begin(), clients.end(), client) ==
                clients.end()) {
                clients.push_back(client);
            }
            if (clients.size() == max_clients) {
                break;
            }
        }
    }

    sf::Packet seed;
    seed << create_seed();
    for (auto& client : clients) {
        if (data.socket.send(seed, client, data.port) != sf::Socket::Done) {
            std::cerr << "Failed to send packet" << std::endl;
            return MESSENGE_SEND_ERROR;
        }
    }

    unsigned seed_value;
    seed >> seed_value;
    Network game(30, 16, 99, &data);
    std::thread(static_cast<int (Network::*)(std::vector<sf::IpAddress>&)>(
                    &Network::recv_data),
                &game, std::ref(clients))
        .detach();
    game.play_multi(clients, seed_value);

    return 0;
}

unsigned Menu::create_seed() {
    std::random_device rd;
    return rd();
}
