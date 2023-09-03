#pragma once
#define BOARD_SIZE 4
#if defined(_WIN32) || defined(_WIN64)
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#include <iostream>
#include <random>
#include <array>
#include <vector>
#include <fstream>
#include <functional>

class Game2048 {
private:
    std::array<std::array<int, 4>, 4> board;
    std::array<std::array<int, 4>, 4> last_board;
    bool is_changed;
    bool is_first_move;
    std::mt19937 gen;

    std::vector<std::pair<int, int>> find_zero_indexes();
    bool check_game_over();
    void process_command(char cmd);
    bool get_user_response(const std::string& prompt);
    void spawn_random_number();
    void print_board();
    void save_game();
    bool load_game();

public:
    Game2048();
    void run();
};
