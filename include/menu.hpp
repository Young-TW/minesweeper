#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Menu {
private:
    std::vector<std::vector<int>> difficulty = {
        {9, 9, 10},
        {16, 16, 40},
        {30, 16, 99}
    };
    std::vector<std::string> difficulty_name = {
        "Easy",
        "Medium",
        "Hard"
    };

    sf::Event event;
    sf::RenderWindow window;
    sf::Font font;
    sf::Text title;
    sf::Text difficulty_text;
    sf::Texture difficulty_texture;
    sf::Sprite difficulty_sprite;

    int draw_difficulty();
    int draw_menu();
    int get_input();

public:
    Menu();

    int run();
};

#endif