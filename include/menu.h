#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>

enum menu_mode_t {
    SINGLE_PLAYER,
    HOST,
    JOIN,
    QUIT
};

enum menu_difficulty_t {
    EASY,
    MEDIUM,
    HARD
};

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
    int draw_button(float place, std::string text);
    int mode_select(int input);
    int get_input();

public:
    Menu();
    class Server;

    std::pair<menu_mode_t, menu_difficulty_t> run();
    menu_mode_t mode_select();
    menu_difficulty_t difficulty_select();
};

#endif