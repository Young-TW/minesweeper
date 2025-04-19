#include <cassert>

#include "menu.h"
#include "host.h"
#include "client.h"
#include "sfml.h"

int main(int argc, char* argv[]) {
    Menu menu;
    auto mode = menu.mode_select(); // menu_mode_t
    auto difficulty = menu.difficulty_select(); // menu_difficulty_t
    switch (mode) {
        case SINGLE_PLAYER: {
            // 先做簡單版
            SFML game(9, 9, 10);
            game.play_single();
            break;
        }
        case HOST: {
            host();
            break;
        }
        case JOIN: {
            client();
            break;
        }
        case QUIT: {
            // 退出遊戲
            break;
        }
        default: {
            // 這裡不應該發生
            assert(false && "未知的遊戲模式");
            break;
        }
    }
    return 0;
}
