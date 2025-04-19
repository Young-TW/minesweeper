#ifndef BOARD_H
#define BOARD_H

#include <chrono>
#include <vector>

#include "block.h"

class Board {
public:
    Board(unsigned row, unsigned col, int mines);
    ~Board();

    int start_game();

protected:
    int generate_mines();
    int clear();
    int show_all_mine();
    int get_input();
    int print_board();
    int reveal(block& target_block);
    int flip_flag(block& target_block);
    int check_win();

    enum game_status { PLAYING, WON, LOST };
    int row = 0;
    int col = 0;
    int n_mines;
    int n_flags = 0;
    int status = PLAYING;
    int n_revealed = 0;

    bool first_move = true;

    std::vector<block> blocks;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
};

#endif