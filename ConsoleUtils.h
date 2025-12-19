#pragma once
#include <string>
#include <vector>

// Коды клавиш
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_PGUP = 73;
const int KEY_PGDN = 81;
const int KEY_ENTER = 13;
const int KEY_ESC = 27;

enum ConsoleColor {
    BLACK = 0,
    GREEN = 2,
    RED = 4,
    WHITE = 7,
    BG_WHITE_FG_BLACK = 240
};

class ConsoleUtils {
public:
    static void setColor(int color);
    static void hideCursor();
    static void showCursor();
    static void clear();
    static void pause();
    static void printError(const std::string& msg);
    static void printSuccess(const std::string& msg);
    static void printHeader(const std::string& title);
    // Меню возвращает индекс выбранного элемента
    static int showMenu(const std::string& title, const std::vector<std::string>& options);
    static int showMenuRunCodeAnalysis(const std::string& title, const std::vector<std::string>& options,const std::string line);
};