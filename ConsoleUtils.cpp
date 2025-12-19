#define _CRT_SECURE_NO_WARNINGS
#include "ConsoleUtils.h"
#include <windows.h>
#include <iostream>
#include <conio.h>

void ConsoleUtils::setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void ConsoleUtils::hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void ConsoleUtils::showCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void ConsoleUtils::clear() {
    system("cls");
}

void ConsoleUtils::pause() {
    setColor(WHITE);
    std::cout << "\nНажмите любую клавишу для продолжения...";
    _getch();
    clear();
}

void ConsoleUtils::printError(const std::string& msg) {
    setColor(RED);
    std::cout << msg << std::endl;
    setColor(WHITE);
}

void ConsoleUtils::printSuccess(const std::string& msg) {
    setColor(GREEN);
    std::cout << msg << std::endl;
    setColor(WHITE);
}

void ConsoleUtils::printHeader(const std::string& title) {
    setColor(WHITE);
    std::cout << "=== " << title << " ===\n\n";
}

int ConsoleUtils::showMenu(const std::string& title, const std::vector<std::string>& options) {
    int selected = 0;
    int numOptions = static_cast<int>(options.size());

    while (true) {
        clear();
        printHeader(title);

        for (int i = 0; i < numOptions; ++i) {
            if (i == selected) {
                setColor(BG_WHITE_FG_BLACK);
                std::cout << " > " << options[i] << " \n";
                setColor(WHITE);
            }
            else {
                std::cout << "   " << options[i] << " \n";
            }
        }

        int key = _getch();
        if (key == 224) { // Спецсимволы
            key = _getch();
            if (key == KEY_UP || key == KEY_PGUP) {
                selected--;
                if (selected < 0) selected = numOptions - 1;
            }
            else if (key == KEY_DOWN || key == KEY_PGDN) {
                selected++;
                if (selected >= numOptions) selected = 0;
            }
        }
        else if (key == KEY_ENTER) {
            return selected;
        }
    }
}




int ConsoleUtils::showMenuRunCodeAnalysis(const std::string& title, const std::vector<std::string>& options, const std::string line) {
    int selected = 0;
    int numOptions = static_cast<int>(options.size());

    while (true) {
        clear();
        printHeader(title);
        ConsoleUtils::setColor(GREEN);
        std::cout << line << std::endl;
        ConsoleUtils::setColor(WHITE);
        for (int i = 0; i < numOptions; ++i) {
            if (i == selected) {
                setColor(BG_WHITE_FG_BLACK);
                std::cout << " > " << options[i] << " \n";
                setColor(WHITE);
            }
            else {
                std::cout << "   " << options[i] << " \n";
            }
        }

        int key = _getch();
        if (key == 224) { // Спецсимволы
            key = _getch();
            if (key == KEY_UP || key == KEY_PGUP) {
                selected--;
                if (selected < 0) selected = numOptions - 1;
            }
            else if (key == KEY_DOWN || key == KEY_PGDN) {
                selected++;
                if (selected >= numOptions) selected = 0;
            }
        }
        else if (key == KEY_ENTER) {
            return selected;
        }
    }
}