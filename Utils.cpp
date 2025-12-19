#include "Utils.h"
#include <conio.h>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>

namespace Utils {

    void setColor(int colorAttr) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorAttr);
    }

    void clearScreen() {
        system("cls");
    }

    void pause() {
        setColor(WHITE);
        std::cout << "\nНажмите любую клавишу для продолжения...";
        _getch();
        clearScreen();
    }

    size_t hashPassword(const std::string& password, const std::string& salt) {
        std::hash<std::string> hasher;
        return hasher(password + salt);
    }

    std::string generateSalt() {
        const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::string salt;
        std::mt19937 generator(static_cast<unsigned int>(time(0)));
        std::uniform_int_distribution<> distribution(0, static_cast<int>(chars.size()) - 1);
        for (int i = 0; i < 16; ++i) salt += chars[distribution(generator)];
        return salt;
    }

    std::string inputPasswordMasked() {
        std::string password;
        char ch;
        while ((ch = _getch()) != KEY_ENTER) {
            if (ch == 8) { // Backspace
                if (!password.empty()) {
                    std::cout << "\b \b";
                    password.pop_back();
                }
            }
            else {
                std::cout << '*';
                password += ch;
            }
        }
        std::cout << std::endl;
        return password;
    }

    std::string toLowerCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    bool isValidLogin(const std::string& login) {
        if (login.empty()) return false;
        for (char c : login) {
            if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == ' ')) {
                return false;
            }
        }
        return true;
    }

    int getIntInput(int min, int max) {
        while (true) {
            std::string line;
            std::getline(std::cin, line);

            // Проверка на пустоту
            if (line.empty()) {
                printError("Ввод не может быть пустым. Повторите ввод: ");
                continue;
            }

            // Проверка: строка должна содержать только цифры (и возможный минус)
            bool isNumber = true;
            size_t start = (line[0] == '-') ? 1 : 0;
            if (start == 1 && line.size() == 1) isNumber = false;

            for (size_t i = start; i < line.size(); ++i) {
                if (!isdigit(line[i])) {
                    isNumber = false;
                    break;
                }
            }

            if (!isNumber) {
                printError("Ошибка! Введите только целое число (без букв и пробелов): ");
                continue;
            }

            try {
                int value = std::stoi(line);
                if (value >= min && value <= max) {
                    return value;
                }
                else {
                    printError("Число вне диапазона [" + std::to_string(min) + " - " + std::to_string(max) + "]. Повторите: ");
                }
            }
            catch (...) {
                printError("Слишком большое число или неверный формат. Повторите: ");
            }
        }
    }

    std::string getStringInput() {
        std::string line;
        while (true) {
            std::getline(std::cin, line);
            if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) {
                printError("Ввод не может быть пустым. Повторите: ");
            }
            else {
                return line;
            }
        }
    }

    void printHeader(const std::string& text) {
        setColor(WHITE);
        std::cout << "\n=== " << text << " ===\n\n";
    }

    void printError(const std::string& text) {
        setColor(RED);
        std::cout << text;
        setColor(WHITE);
    }

    void printSuccess(const std::string& text) {
        setColor(GREEN);
        std::cout << text << std::endl;
        setColor(WHITE);
    }

    int showMenu(const std::string& title, const std::vector<std::string>& options) {
        int selected = 0;
        int numOptions = static_cast<int>(options.size());

        while (true) {
            clearScreen();
            printHeader(title);

            for (int i = 0; i < numOptions; ++i) {
                if (i == selected) {
                    setColor(BG_WHITE_FG_BLACK); // Белый фон, черный текст
                    std::cout << " > " << options[i] << " \n";
                    setColor(WHITE);
                }
                else {
                    std::cout << "   " << options[i] << " \n";
                }
            }

            int key = _getch();
            if (key == 224) { // Спецсимвол (стрелки)
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
}