#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

class Menu {
private:
    std::vector<std::vector<int>> difficulty = {
        {9, 9, 10}, {16, 16, 40}, {30, 16, 99}};
    std::vector<std::vector<std::string>> mode = {{"Single", "Multiplayer"},
                                                  {"Host", "Join"},
                                                  {"Easy", "Medium", "Hard"}};

    sf::RenderWindow window;
    sf::Font font;
    sf::Text title;
    sf::Text difficulty_text;
    sf::Text menu_text;
    sf::Texture texture;
    sf::Sprite sprite;
    int mode_index = 0;

    int draw_menu();
    int draw_button(int button_index, bool is_pressed = false);
    int mode_select(int input);
    int get_input();
    int host();
    int client();
    unsigned create_seed();

public:
    Menu();
    class Server;

    int run();
};

#endif