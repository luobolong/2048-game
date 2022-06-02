#define NOMINMAX
#include <random>
#include <vector>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <windows.h>

void change_font()
{
    static CONSOLE_FONT_INFOEX  fontex;
    fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetCurrentConsoleFontEx(hOut, 0, &fontex);
    fontex.FontWeight = 700;
    fontex.dwFontSize.X = 30;
    fontex.dwFontSize.Y = 30;
    SetCurrentConsoleFontEx(hOut, NULL, &fontex);
}

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

bool generate_randomly(int arr[4][4], bool is_changed) {
    /*
    * Before generating a new number, it's known that the vector is never empty,
    * that is, the vector is possibly empty only after generating a new number.
    *   empty(after generation)     is_changed      behavior
    *   0                           0               Do not generate a new number and return true.
    *   0                           1               Generate a new number and return true.
    *   1                           0               Do not generate a new number and return true.
    *   1                           1               Check if failed. If failed return false else return true.
    */
    std::vector<std::pair<int, int>> zero_indexes;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j) {
            if (arr[i][j] == 0) {
                std::pair<int, int> idx;
                idx.first = i;
                idx.second = j;
                zero_indexes.push_back(idx);
            }
        }
    }
    if (!is_changed)
    {
        return true;
    }
    auto r_iter = select_randomly(zero_indexes.begin(), zero_indexes.end());
    std::pair<int, int> r_idx = *r_iter;
    zero_indexes.erase(r_iter);

    std::random_device rd;
    std::uniform_int_distribution<int> dist(1, 100);
    std::mt19937 gen(rd());
    int r = dist(gen);
    if (r > 75)
    {
        arr[r_idx.first][r_idx.second] = 4;
    }
    else
    {
        arr[r_idx.first][r_idx.second] = 2;
    }
    if (zero_indexes.empty())
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (arr[i][j] == arr[i][j + 1])
                {
                    return true;
                }
                if (arr[j][i] == arr[j + 1][i])
                {
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}

void print(int arr[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (arr[i][j] != 0) {
                std::cout << arr[i][j];
            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

int main()
{
    change_font();
    int arr[4][4]{ 0 };
    bool is_changed = true;
    generate_randomly(arr, is_changed);

    int last_arr[4][4];
    memcpy(&last_arr, &arr, sizeof last_arr);

    for (;;)
    {
        system("CLS");
        if (!generate_randomly(arr, is_changed))
        {
            print(arr);
            std::cout << "FAILED\n";
            for (;;)
            {
                std::cout << "Return?(y/n)";
                char response;
                std::cin >> response;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (response == 'n')
                {
                }
                else if (response == 'y')
                {
                    memcpy(&arr, &last_arr, sizeof arr);
                    is_changed = false;
                    break;
                }
                else
                {
                    continue;
                }
                std::cout << "Restart?(y/n)";
                std::cin >> response;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                response = tolower(response);
                if (response == 'n')
                {
                    return 0;
                }
                else if (response == 'y')
                {
                    memset(arr, 0, sizeof(int) * 4 * 4);
                    is_changed = true;
                    generate_randomly(arr, is_changed);
                    memcpy(&last_arr, &arr, sizeof last_arr);
                    break;
                }
            }
            continue;
        }
        is_changed = false;
        print(arr);
        for (bool is_valid = false; !is_valid;)
        {
            char cmd;
            std::cin >> cmd;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cmd = tolower(cmd);
            int tmp[4][4];
            memcpy(&tmp, &arr, sizeof tmp);
            switch (cmd) {
            case 'w':
                for (int i = 0; i < 4; ++i)
                {
                    int column[4]{ 0 };
                    int *ptr = column;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[j][i] == 0)
                        {
                            continue;
                        }
                        if (ptr == column && *ptr == 0)
                        {
                            *ptr = arr[j][i];
                            continue;
                        }
                        if (arr[j][i] == *ptr)
                        {
                            *ptr *= 2;
                            ptr++;
                        }
                        else
                        {
                            if (*ptr != 0)
                            {
                                ptr++;
                            }
                            *ptr = arr[j][i];
                        }
                    }
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[j][i] != column[j]) {
                            is_changed = true;
                            tmp[j][i] = column[j];
                        }
                    }
                }
                if (is_changed)
                {
                    memcpy(&last_arr, &arr, sizeof last_arr);
                    memcpy(&arr, &tmp, sizeof last_arr);
                }
                is_valid = true;
                break;
            case 'a':
                for (int i = 0; i < 4; ++i)
                {
                    int row[4]{ 0 };
                    int *ptr = row;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[i][j] == 0)
                        {
                            continue;
                        }
                        if (ptr == row && *ptr == 0)
                        {
                            *ptr = arr[i][j];
                            continue;
                        }
                        if (arr[i][j] == *ptr)
                        {
                            *ptr *= 2;
                            ptr++;
                        }
                        else
                        {
                            if (*ptr != 0)
                            {
                                ptr++;
                            }
                            *ptr = arr[i][j];
                        }
                    }
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[i][j] != row[j]) {
                            is_changed = true;
                            tmp[i][j] = row[j];
                        }
                    }
                }
                if (is_changed)
                {
                    memcpy(&last_arr, &arr, sizeof last_arr);
                    memcpy(&arr, &tmp, sizeof last_arr);
                }
                is_valid = true;
                break;
            case 's':
                for (int i = 0; i < 4; ++i)
                {
                    int column[4]{ 0 };
                    int *ptr = column;
                    for (int j = 3; j >= 0; --j)
                    {
                        if (arr[j][i] == 0)
                        {
                            continue;
                        }
                        if (ptr == column && *ptr == 0)
                        {
                            *ptr = arr[j][i];
                            continue;
                        }
                        if (arr[j][i] == *ptr)
                        {
                            *ptr *= 2;
                            ptr++;
                        }
                        else
                        {
                            if (*ptr != 0)
                            {
                                ptr++;
                            }
                            *ptr = arr[j][i];
                        }
                    }
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[j][i] != column[3 - j]) {
                            is_changed = true;
                            tmp[j][i] = column[3 - j];
                        }
                    }
                }
                if (is_changed)
                {
                    memcpy(&last_arr, &arr, sizeof last_arr);
                    memcpy(&arr, &tmp, sizeof last_arr);
                }
                is_valid = true;
                break;
            case 'd':
                for (int i = 0; i < 4; ++i)
                {
                    int row[4]{ 0 };
                    int *ptr = row;
                    for (int j = 3; j >= 0; --j)
                    {
                        if (arr[i][j] == 0)
                        {
                            continue;
                        }
                        if (ptr == row && *ptr == 0)
                        {
                            *ptr = arr[i][j];
                            continue;
                        }
                        if (arr[i][j] == *ptr)
                        {
                            *ptr *= 2;
                            ptr++;
                        }
                        else
                        {
                            if (*ptr != 0)
                            {
                                ptr++;
                            }
                            *ptr = arr[i][j];
                        }
                    }
                    for (int j = 0; j < 4; ++j)
                    {
                        if (arr[i][j] != row[3 - j]) {
                            is_changed = true;
                            tmp[i][j] = row[3 - j];
                        }
                    }
                }
                if (is_changed)
                {
                    memcpy(&last_arr, &arr, sizeof last_arr);
                    memcpy(&arr, &tmp, sizeof last_arr);
                }
                is_valid = true;
                break;
            case 'q':
                is_valid = true;
                return 0;
                break;
            case 'r':
                memcpy(&arr, &last_arr, sizeof arr);
                is_changed = false;
                is_valid = true;
                break;
            default:
                std::cout << "Invalid command. Please input your command again.\n";
                break;
            }
        }
    }
    return 0;
}

