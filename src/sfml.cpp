#include "sfml.hpp"

#include <iostream>

#include "config.h"

SFML::SFML(unsigned row, unsigned col, int mines) : Board(row, col, mines) {
    unsigned int window_scale = 16 * blockScale;
    window.create(
        sf::VideoMode({row * window_scale, col * window_scale + title_height}),
        "Minesweeper");
    window.setFramerateLimit(60);

    if (!window.isOpen()) {
        std::cerr << "Failed to create window" << std::endl;
    }
    if (!font.openFromFile("../font/Cubic_11_1.100_R.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    if (!texture.loadFromFile("../image/spritesheet.png")) {
        std::cerr << "Failed to load texture" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale({blockScale, blockScale});

    face.setTexture(texture);
    face.setScale({blockScale, blockScale});
    face.setPosition(
        {static_cast<float>(window.getSize().x) / 2 - 26.0f, 4.0f});
    face.setTextureRect(sf::IntRect({0, 24}, {26, 26}));

    number.setTexture(texture);
    number.setScale({blockScale, blockScale});
}

int SFML::init_block() {
    int pos = std::min(window.getSize().x / row, window.getSize().y / col);
    for (block &block : blocks) {
        block.gl_x = block.index % row * pos;
        block.gl_y = block.index / row * pos + title_height;
    }

    return 0;
}

int SFML::draw_board() {
    for (int i = 0; i < row * col; i++) {
        if (blocks[i].state == block::REVEALED) {
            if (blocks[i].value >= block::MINE) {
                if (get_block() == i && status == LOST) {
                    sprite.setTextureRect(sf::IntRect({102, 51}, {16, 16}));
                } else {
                    sprite.setTextureRect(sf::IntRect({85, 51}, {16, 16}));
                }
            } else if (blocks[i].value != block::EMPTY) {
                int sprite_pos = (blocks[i].value - 1) * 17;
                sprite.setTextureRect(sf::IntRect({sprite_pos, 68}, {16, 16}));
            } else {
                sprite.setTextureRect(sf::IntRect({17, 51}, {16, 16}));
            }
        } else if (blocks[i].state == block::FLAGGED) {
            if (status == LOST && blocks[i].value != block::MINE) {
                sprite.setTextureRect(sf::IntRect({119, 51}, {16, 16}));
            } else {
                sprite.setTextureRect(sf::IntRect({34, 51}, {16, 16}));
            }
        } else {
            sprite.setTextureRect(sf::IntRect({0, 51}, {16, 16}));
        }
        sprite.setPosition({blocks[i].gl_x, blocks[i].gl_y});
        window.draw(sprite);
        window.draw(face);
    }

    return 0;
}

int SFML::draw_flag() {
    int need_flag = n_mines - n_flags;
    for (int i = 100, digit, j = 0; i; i /= 10, j++) {
        digit = (need_flag / i + 9) % 10;
        number.setTextureRect(sf::IntRect({digit * 14, 0}, {13, 23}));
        number.setPosition({4.0f + j * 26.0f, 7.0f});
        window.draw(number);
    }

    return 0;
}

int SFML::draw_time() {
    std::chrono::duration<double> elapsed_time;
    if (first_move) {
        elapsed_time = std::chrono::duration<double>::zero();
    } else if (status == PLAYING) {
        elapsed_time = std::chrono::system_clock::now() - start_time;
    } else {
        elapsed_time = end_time - start_time;
    }

    for (int i = 100, digit, j = 3; i; i /= 10, j--) {
        digit = ((int)elapsed_time.count() / i + 9) % 10;
        number.setTextureRect(sf::IntRect({digit * 14, 0}, {13, 23}));
        number.setPosition(
            {static_cast<float>(window.getSize().x) - 4.0f - j * 26.0f, 7.0f});
        window.draw(number);
    }

    return 0;
}

std::pair<SFML::game_action, int> SFML::mouse_input() {
    sf::Event event;
    auto action = std::make_pair(SFML::NONE, -1);

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            int index = get_block();
            if (index < 0 || index >= row * col) {
                action.second = index;
                break;
            }
            if (event.mouseButton.button == sf::Mouse::Left) {
                face.setTextureRect(sf::IntRect({54, 24}, {26, 26}));
                window.draw(face);
                reveal(blocks[index]);
                action = {SFML::REVEAL, index};
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                if (blocks[index].state == block::HIDDEN ||
                    blocks[index].state == block::FLAGGED) {
                    flip_flag(blocks[index]);
                    action = {SFML::FLAG, index};
                }
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            face.setTextureRect(sf::IntRect({0, 24}, {26, 26}));
            window.draw(face);
        }
    }

    return action;
}

int SFML::get_block() {
    sf::Vector2i pos = sf::Mouse::getPosition(window);
    if (pos.y < title_height) {
        if (pos.x > window.getSize().x / 2 - 26 &&
            pos.x < window.getSize().x / 2 + 26) {
            face.setTextureRect(sf::IntRect({27, 24}, {26, 26}));
            window.draw(face);

            return -2;
        }
        return -1;
    }

    int x = pos.x / (window.getSize().x / row);
    int y = (pos.y - title_height) / (window.getSize().x / row);

    return x + y * row;
}

int SFML::play_single() {
    init_block();
    generate_mines();
    while (window.isOpen() && status == PLAYING) {
        window.clear(sf::Color::Black);
        draw_board();
        draw_flag();
        draw_time();
        window.display();

        if (mouse_input().second == -2) {
            clear();
            generate_mines();
        }
        check_win();
    }
    end_game();

    return 0;
}

int SFML::end_game() {
    if (status == WON) {
        face.setTextureRect(sf::IntRect({81, 24}, {26, 26}));
        for (int i = 0; i < row * col; i++) {
            if (blocks[i].value >= block::MINE) {
                blocks[i].state = block::FLAGGED;
            }
        }
    } else {
        face.setTextureRect(sf::IntRect({108, 24}, {26, 26}));
        for (int i = 0; i < row * col; i++) {
            if (blocks[i].value >= block::MINE) {
                blocks[i].state = block::REVEALED;
            }
        }
    }

    draw_board();
    window.display();

    sf::sleep(sf::seconds(1));
    sf::Event event;
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            if (get_block() == -2) {
                clear();
                play_single();
                break;
            }
        }
    }
    return 0;
}