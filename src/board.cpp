#include "board.h"

#include <random>
#include <iostream>
#include <chrono>

Board::Board(uint8_t row, uint8_t col, uint16_t n_mines):
       row(row),
       col(col),
       n_mines(n_mines),
       n_flags(0),
       revealed(0),
       status(PLAYING),
       board(row * col, 0) {

    // Generate Mines Locations

    std::random_device rd;  // 取得隨機數種子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<uint16_t> dis(0, this->row * this->col - 1);
    for (int i=0; i<this->n_mines; i++) {
        uint16_t rand = dis(gen);
        if (this->board[rand] == 9) {
            i--;
            continue;
        }

        this->board[rand] = 9;
        for (int j=-1; j<=1; j++) {
            if (rand < row && j == -1) continue;
            if (rand >= row * (col - 1) && j == 1) continue;

            for (int k = -1; k <= 1; k++) {
                if (rand % row == 0 && k == -1) continue;
                if (rand % row == row - 1 && k == 1) continue;
                this->board[rand + j * row + k]++;
            }
        }
    }

    this->blocks.resize(row * col);
    for (int16_t i=0; i<row * col; i++) {
        auto temp_x = i%col;
        auto temp_y = i/col;
        this->blocks[i].x = temp_x*this->blocks[i].size + (temp_x)*this->border;
        this->blocks[i].y = temp_y*this->blocks[i].size + (temp_y)*this->border;
        std::cout << "blocks[" << i << "].x = " << this->blocks[i].x << std::endl;
        std::cout << "blocks[" << i << "].y = " << this->blocks[i].y << std::endl;
    }
}

Board::~Board() {}

int Board::show_all_mine() {
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].value == 9 && blocks[i].state != 1) {
            blocks[i].state = 1;
            std::cout << "Revealed mine at position: (" << blocks[i].x << ", " << blocks[i].y << ")\n";
        }
        status=LOST;
    }
    return 0;
}

int Board::timer() {
    auto startpoint = std::chrono::steady_clock::now();
    if (status == LOST || status == WON) {
        auto endpoint = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration elapsed = endpoint - startpoint;
        double elapsed_seconds = std::chrono::duration_cast<std::chrono::millisecond>(elapsed).count();
        std::cout << "End time: (" << elapsed_seconds << "s)\n";
    }
    return 0;
}


