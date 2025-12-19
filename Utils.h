#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>

namespace Utils {
    // Константы клавиш
    const int KEY_UP = 72;
    const int KEY_DOWN = 80;
    const int KEY_PGUP = 73;   // PageUp
    const int KEY_PGDN = 81;   // PageDown
    const int KEY_ENTER = 13;
    const int KEY_ESC = 27;

    // Цвета
    enum ConsoleColor {
        BLACK = 0,
        GREEN = 2,
        RED = 4,
        WHITE = 7,
        BG_WHITE_FG_BLACK = 240 // Белый фон, черный текст
    };

    void setColor(int colorAttr);
    void clearScreen();
    void pause();

    // Хеширование и безопасность
    size_t hashPassword(const std::string& password, const std::string& salt);
    std::string generateSalt();
    std::string inputPasswordMasked();

    // Ввод и валидация
    std::string toLowerCase(std::string str);
    bool isValidLogin(const std::string& login);
    int getIntInput(int min, int max);
    std::string getStringInput();

    // Интерфейс
    void printHeader(const std::string& text);
    void printError(const std::string& text);
    void printSuccess(const std::string& text);
    int showMenu(const std::string& title, const std::vector<std::string>& options);
}