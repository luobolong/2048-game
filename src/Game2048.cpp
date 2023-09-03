#include "Game2048.h"

/**
 * @brief Finds the indices of all empty cells (cells with a value of zero) in the game board.
 *
 * This function iterates over the entire game board and records the positions where the value is zero,
 * indicating an empty cell. These positions are returned as a vector of pairs, where each pair
 * contains the row and column index of the zero value.
 *
 * @return std::vector<std::pair<int, int>> Vector containing row-column pairs for each zero value on the board.
 */
std::vector<std::pair<int, int>> Game2048::find_zero_indexes() {
    std::vector<std::pair<int, int>> zero_indexes;
    // Iterating over each row and column
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // If cell value is zero, store its indices
            if (board[i][j] == 0) {
                zero_indexes.emplace_back(i, j);
            }
        }
    }
    return zero_indexes;
}

/**
 * @brief Checks if the game is over based on the current board state.
 *
 * This function evaluates the board to determine if any more moves can be made.
 * Initially, it checks if there are any empty cells. If so, the game is not over.
 * Then it checks for adjacent cells that have the same value, either horizontally or vertically,
 * indicating that a move can be made. If none are found, the function returns true, indicating the game is over.
 *
 * @return bool True if the game is over, otherwise false.
 */
bool Game2048::check_game_over() {
    auto zero_indexes = find_zero_indexes();
    // If there are zero positions, the game can continue
    if (!zero_indexes.empty()) {
        return false;
    }

    // Check for equal adjacent numbers horizontally and vertically
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE - 1; ++j) {
            // If horizontally or vertically adjacent cells are equal, game can continue
            if (board[i][j] == board[i][j + 1] || board[j][i] == board[j + 1][i]) {
                return false;
            }
        }
    }

    // If no zeros or adjacent equal numbers are found, the game is over
    return true;
}

/**
 * @brief Processes a single character command to move the board.
 *
 * This function takes a single character representing the direction to move
 * (e.g., 'w' for moving up, 'a' for moving left, etc.). It then updates the board state
 * based on this command.
 *
 * @param cmd A char representing the command (direction to move).
 */
void Game2048::process_command(char cmd) {
    // Temporary board to store changes
    auto tmp = board;

    /**
     * @brief Lambda function to perform a board move.
     *
     * This lambda function is responsible for performing the actual move
     * on the board. It takes four arguments:
     * - start: the starting index
     * - end: the ending index
     * - step: the step size (can be negative for reverse)
     * - action: a lambda function that actually performs the moving and combining of tiles
     */
    auto move_board = [&](int start, int end, int step,
                            std::function<int&(std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE>&, int, int)> action) {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            int subarray[BOARD_SIZE]{0};
            int *ptr = subarray;

            // Iterating through the board to perform the move
            for (int j = start; j != end; j += step) {
                int val = action(board, i, j);
                if (val == 0) continue;

                // Logic for combining or moving tiles
                if (ptr == subarray && *ptr == 0) {
                    *ptr = val;
                    continue;
                }
                if (val == *ptr) {
                    *ptr *= 2;
                    ptr++;
                } else {
                    if (*ptr != 0) ptr++;
                    *ptr = val;
                }
            }

            // Update board if any change is detected
            for (int j = start, k = 0; j != end; j += step, ++k) {
                if (action(board, i, j) != subarray[k]) {
                    is_changed = true;
                    action(tmp, i, j) = subarray[k];
                }
            }
        }
    };
    
    // Switch statement to handle different commands
    switch (cmd) {
        case 'w':
            // Move upwards
            move_board(0, BOARD_SIZE, 1, [](std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> &a, int x, int y) -> int& { return a[y][x]; });
            break;
        case 'a':
            // Move to the left
            move_board(0, BOARD_SIZE, 1, [](std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> &a, int x, int y) -> int& { return a[x][y]; });
            break;
        case 's':
            // Move downwards
            move_board(BOARD_SIZE - 1, -1, -1, [](std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> &a, int x, int y) -> int& { return a[y][x]; });
            break;
        case 'd':
            // Move to the right
            move_board(BOARD_SIZE - 1, -1, -1, [](std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> &a, int x, int y) -> int& { return a[x][y]; });
            break;
        default:
            // Invalid command, do nothing
            return;
    }

    // Update the board state if it has changed
    if (is_changed) {
        last_board = board;
        board = tmp;
    }
}

/**
 * @brief Prompts the user for a 'yes' or 'no' input and returns the corresponding boolean value.
 *
 * This function repeatedly prompts the user for input until a valid 'y' or 'n' is received.
 * 'y' will return true, 'n' will return false.
 *
 * @param prompt A string representing the message to display to the user.
 * @return bool Returns true if user inputs 'y', false if user inputs 'n'.
 */
bool Game2048::get_user_response(const std::string& prompt) {
    char response;
    for (;;) {  // Infinite loop until valid input is received
        std::cout << prompt;
        std::cin >> response;

        // Clear input stream
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Convert to lowercase to make the check case-insensitive
        response = tolower(response);

        if (response == 'y') return true;
        if (response == 'n') return false;

        // Invalid input received; ask for input again
        std::cout << "Invalid input. Please try again.\n";
    }
}

/**
 * @brief Spawns a new random number (either 2 or 4) in an empty cell on the board.
 *
 * This function only spawns a new number if certain conditions are met:
 * - The board has changed since the last move (`is_changed`)
 * - There are empty cells available on the board (`zero_indexes`)
 */
void Game2048::spawn_random_number() {
    // Find the indexes where the board has zero (empty cell)
    auto zero_indexes = find_zero_indexes();

    // If the board has not changed or there are no zeros, return without spawning a new number
    if (!is_changed || zero_indexes.empty()) {
        return;
    }

    // Generate a random index within the range of zero_indexes
    std::uniform_int_distribution<int> dist(0, zero_indexes.size() - 1);
    int random_index = dist(gen);

    // Select the cell based on the random index
    auto selected_cell = zero_indexes[random_index];

    // Remove the selected index from zero_indexes for future use
    std::swap(zero_indexes[random_index], zero_indexes.back());
    zero_indexes.pop_back();

    // Randomly place either a 2 (75% chance) or a 4 (25% chance) in the selected cell
    std::uniform_int_distribution<int> value_dist(0, 3);
    board[selected_cell.first][selected_cell.second] = (value_dist(gen) == 0) ? 4 : 2;
}

/**
 * @brief Prints the current state of the 2048 board to the console.
 *
 * This function iterates through each cell in the 2048 board. If the cell is not empty (zero),
 * it prints the cell's value. Otherwise, it prints a dot '.' to represent an empty cell.
 */
void Game2048::print_board() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // Print the value if it's not zero, otherwise print a dot.
            if (board[i][j] != 0) {
                std::cout << board[i][j];
            } else {
                std::cout << ".";
            }
            std::cout << "\t";  // Tab for formatting
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Saves the current game state to a binary file named "data.bin".
 *
 * This function opens a binary file for writing and writes the state of each cell in the board.
 * It reports an error to the standard error stream if the file cannot be opened.
 */
void Game2048::save_game() {
    std::ofstream outfile("data.bin", std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file for writing data.bin" << std::endl;
        return;
    }
    // Write each row of the board to the file
    for (const auto &row : board) {
        outfile.write(reinterpret_cast<const char *>(row.data()), sizeof(int) * BOARD_SIZE);
    }
    outfile.close();
}

/**
 * @brief Loads the game state from a binary file named "data.bin".
 *
 * This function opens a binary file for reading and updates the board state.
 * It returns false and reports an error if the file cannot be opened, otherwise returns true.
 *
 * @return bool True if the game state was successfully loaded, false otherwise.
 */
bool Game2048::load_game() {
    std::ifstream infile("data.bin", std::ios::in | std::ios::binary);
    
    if (!infile.good()) {
        std::cerr << "Error: Cannot open \"data.bin\" for reading." << std::endl;
        return false;
    }

    // Read each row of the board from the file
    for (auto &row : board) {
        infile.read(reinterpret_cast<char *>(row.data()), sizeof(int) * BOARD_SIZE);
    }

    infile.close();
    return true;
}

/**
 * @brief Default constructor for the Game2048 class. Initializes game board and spawns initial number.
 * 
 * This constructor initializes all board cells to zero and spawns an initial random number on the board.
 */
Game2048::Game2048() : is_changed(true), is_first_move(true), gen(std::random_device()()) {
    // Initialize all elements in each row of the board to zero
    for (auto &row : board) {
        row.fill(0);
    }

    // Make a copy of the initial state of the board.
    last_board = board;

    // Spawn an initial random value on the board.
    spawn_random_number();
}

/**
 * @brief The main game loop function that runs the 2048 game.
 *
 * This function handles the game's logic, updates the board, receives and processes user inputs, 
 * and checks for game-over conditions.
 */
void Game2048::run() {
    // Store the last board state
    last_board = board;

    // Main game loop
    for (;;) {
        // Clear screen
        system(CLEAR);

        // Spawn a new random number on the board
        spawn_random_number();

        // Reset the change flag
        is_changed = false;

        // Display the current board state
        print_board();

        // Check for game over condition
        if (check_game_over()) {
            std::cout << "FAILED\n";

            // Ask if the user wants to roll back to the last state
            if (get_user_response("Return?(y/n)")) {
                board = last_board;
                is_changed = false;
                continue;
            }

            // Ask if the user wants to restart the game.
            if (!get_user_response("Restart?(y/n)")) {
                return;
            }

            // Reset board state and reinitialize.
            std::fill(board.begin(), board.end(), std::array<int, BOARD_SIZE>{0});
            is_changed = true;
            spawn_random_number();
            last_board = board;
            continue;
        }

        // Loop for receiving and processing user commands.
        for (bool is_valid = false; !is_valid;) {
            char cmd;
            std::cin >> cmd;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cmd = tolower(cmd);  // Normalize to lowercase

            // Process user command based on input.
            switch (cmd) {
                case 'w':
                case 'a':
                case 's':
                case 'd':
                    process_command(cmd);
                    is_first_move = false;
                    is_valid = true;
                    break;
                case 'q':
                    return;
                case 'r':
                    if (!is_first_move) {
                        board = last_board;
                        is_changed = false;
                    }
                    is_valid = true;
                    break;
                case 'f':
                    save_game();
                    is_valid = true;
                    break;
                case 'g':
                    is_valid = load_game();
                    break;
                case 'h':
                    // Display help menu.
                    std::cout << "h: display help menu\n";
                    std::cout << "w: move upwards\n";
                    std::cout << "a: move leftwards\n";
                    std::cout << "s: move downwards\n";
                    std::cout << "d: move rightwards\n";
                    std::cout << "r: roll back once\n";
                    std::cout << "f: save data file\n";
                    std::cout << "g: load data file\n";
                break;
            default:
                std::cout << "Invalid command. Please input your command again.\n";
                break;
            }
        }
    }
}