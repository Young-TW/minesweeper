#include "menu.h"

#include <iostream>
#include <vector>
#include <string>

Menu::Menu()
    : title(font), difficulty_text(font), menu_text(font), sprite(texture) {
    window.create(sf::VideoMode({260, 320}), "Minesweeper");
    const sf::Image icon("./image/icon/menu.png");
    window.setIcon({icon.getSize().x, icon.getSize().y}, icon.getPixelsPtr());
    if (!font.openFromFile("./font/Cubic_11_1.100_R.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    if (!texture.loadFromFile("./image/menu_button.png")) {
        std::cerr << "Failed to load difficulty.png" << std::endl;
    }

    title.setFont(font);
    title.setString("Minesweeper!");
    title.setCharacterSize(24);
    title.setPosition({60, 20});
    title.setFillColor(sf::Color::White);

    sprite.setTexture(texture);
    sprite.setScale({2.0f, 2.0f});

    difficulty_text.setFont(font);
    difficulty_text.setCharacterSize(16);
    difficulty_text.setFillColor(sf::Color::White);

    menu_text.setFont(font);
    menu_text.setCharacterSize(26);
    menu_text.setFillColor(sf::Color::White);
}

int Menu::draw_button(float place, std::string text) {
    // 設定按鈕背景圖的位置
    sprite.setPosition({0, 80 + place * 60});
    window.draw(sprite);

    // 設定按鈕文字
    menu_text.setString(text);
    menu_text.setPosition({65, 82 + place * 60});
    window.draw(menu_text);

    // ❌ 這裡不應該呼叫 window.display()，這應該在 draw_menu() 統一處理
    return 0;
}


int Menu::draw_menu() {
    window.clear(sf::Color::Black);

    // 畫標題
    window.draw(title);

    // 畫 Exit / Back 鍵（固定在最下方）
    draw_button(3, mode_index == 3 ? "Back" : "Exit");

    // 畫其他選項按鈕
    for (int i = 0; i < mode[mode_index].size(); i++) {
        draw_button(i, mode[mode_index][i]);
    }

    // 顯示畫面（一次就好）
    window.display();

    return 0;
}

int Menu::get_input() {
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if (mouse.y >= 80 && mouse.y <= 132) {
            return 0; // SINGLE_PLAYER or EASY
        } else if (mouse.y >= 140 && mouse.y <= 192) {
            return 1; // HOST or MEDIUM
        } else if (mouse.y >= 200 && mouse.y <= 252) {
            return 2; // JOIN or HARD
        } else if (mouse.y >= 260 && mouse.y <= 312) {
            return 3; // QUIT
        }
    }
    return -1;
}

menu_mode_t Menu::mode_select() {
    while (true) {
        draw_menu();
        int input = get_input();
        if (input == -1) continue;

        if (input == 3) {
            if (mode_index == 0) {
                return QUIT;
            } else {
                mode_index = 0;
                continue;
            }
        }

        switch (mode_index) {
            case 0:  // 主畫面：選單人 / 多人
                if (input == 0) {
                    return SINGLE_PLAYER; // 單人 → 主程式接手叫 difficulty_select()
                } else if (input == 1) {
                    mode_index = 1; // 切換到 Host / Join 畫面
                }
                break;

            case 1:  // 多人選單：Host / Join
                return static_cast<menu_mode_t>(input + 1); // HOST = 1, JOIN = 2
        }
    }
}

menu_difficulty_t Menu::difficulty_select() {
    mode_index = 2; // 強制設為難度選單狀態

    while (true) {
        draw_menu();
        int input = get_input();
        if (input == -1) continue;

        if (input == 3) {
            // 返回上一頁（回主選單）
            return EASY; // 或者丟例外 / 用 std::optional 回傳表示取消
        }

        if (input >= 0 && input <= 2) {
            return static_cast<menu_difficulty_t>(input);
        }
    }
}
