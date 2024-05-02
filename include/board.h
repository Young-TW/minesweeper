#ifndef BOARD_H
#define BOARD_H

#include <vector>

struct block {
    const int size = 50;
    int state = 0; // 0: hidden, 1: revealed, 2: flagged
    int value = 0; // 0: empty, 1-8: number of mines around, 9: mine
};

class Board {
    public:
        enum game_status {
            PLAYING,
            WON,
            LOST
        };

        Board(int row, int col, int mines);
        ~Board();

        int show_all_mine();
        int timer();
        std::pair<int, int> get_input();
        int start_game();
        int print_board();
        int reveal(std::pair<int, int> input);
        int flagged(size_t x, size_t y);
        int remove_flagged(size_t x, size_t y);

        int gl_open_board();
        int gl_reveal(int x, int y);
        int gl_flagged(int x, int y);
        int gl_remove_flagged(int x, int y);
        int gl_show_all_mine();

        const int border = 10;

        int row = 0;
        int col = 0;
        int n_mines;
        int n_flags = 0;
        int status = PLAYING;
        int n_revealed = 0;

        std::vector<block> blocks;
        std::vector<block> revealed_blocks;
};

#endif