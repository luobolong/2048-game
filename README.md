# 2048 Game

This is a simple console-based implementation of the 2048 game, written in C++. I've been feeling down due to work pressures and family concerns. I found solace in refactoring this project as a therapeutic exercise, while it hasn't completely resolved my issues though :(

## Features

- Use the WASD keys for movement
- Ability to roll back to the previous state (`r` key)
- Save game state to a binary file (`f` key)
- Load game state from a binary file (`g` key)

## Requirements

- C++11 or above
- CMake 3.10 or above

## Building the Project

1. Navigate to the project directory in the terminal.
2. Enter the `build` directory:

    ```sh
    cd build
    ```

3. Generate the build files:

    ```sh
    cmake ..
    ```

4. Compile the project:

    ```sh
    make
    ```

## How to Play

1. Navigate to the `build` directory.
2. Run the compiled executable:

    ```sh
    ./Game2048
    ```

3. Use the WASD keys to move the tiles.

## License

This project is open-source, available under the [MIT License](LICENSE).
