#define _CRT_SECURE_NO_WARNINGS
#include "Validator.h"
#include "ConsoleUtils.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <conio.h>

std::string Validator::toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return str;
}

bool Validator::isValidLogin(const std::string& login) {
    if (login.empty()) return false;
    for (char c : login) {
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == ' ')) {
            return false;
        }
    }
    return true;
}

std::string Validator::getStringInput() {
    std::string line;
    ConsoleUtils::showCursor();

    int c;
    while (true) {
        c = _getch();
        if (c == KEY_ESC) {
            ConsoleUtils::hideCursor();
            throw OperationCancelled();
        }
        // Обработка обычного ввода посимвольно для отлова ESC
        // Для упрощения здесь используем getline, но проверяем ESC перед вводом
        // В реальном консольном приложении для идеального UX нужно писать свой getline
        // Но для курсовой сделаем проверку первого символа:

        if (c == '\r' || c == '\n') break; // Enter

        if (c == 8) { // Backspace
            if (!line.empty()) {
                std::cout << "\b \b";
                line.pop_back();
            }
        }
        else if (isprint(c)) {
            std::cout << (char)c;
            line += (char)c;
        }
        else if (c == 0 || c == 224) { _getch(); } // Пропуск стрелок
    }
    std::cout << std::endl;

    if (line.empty() || std::all_of(line.begin(), line.end(), isspace)) {
        // Если пустой ввод (просто Enter), рекурсивно просим еще раз, но это может быть неудобно.
        // Лучше вернуть пустую строку, а валидацию делать снаружи, 
        // но по ТЗ "не допуская пустого ввода":
        ConsoleUtils::printError("Ввод не может быть пустым. Повторите: ");
        return getStringInput();
    }

    ConsoleUtils::hideCursor();
    return line;
}

int Validator::getIntInput(int min, int max) {
    ConsoleUtils::showCursor();
    while (true) {
        std::string line;
        // Читаем посимвольно для ESC
        int c;
        while (true) {
            c = _getch();
            if (c == KEY_ESC) {
                ConsoleUtils::hideCursor();
                throw OperationCancelled();
            }
            if (c == '\r' || c == '\n') break;
            if (c == 8) {
                if (!line.empty()) { std::cout << "\b \b"; line.pop_back(); }
            }
            else if (isdigit(c) || (c == '-' && line.empty())) {
                std::cout << (char)c;
                line += (char)c;
            }
        }
        std::cout << std::endl;

        if (line.empty()) {
            ConsoleUtils::printError("Пустой ввод. Введите число: ");
            continue;
        }

        try {
            int val = std::stoi(line);
            if (val >= min && val <= max) {
                ConsoleUtils::hideCursor();
                return val;
            }
            ConsoleUtils::printError("Число вне диапазона [" + std::to_string(min) + "-" + std::to_string(max) + "]. Повторите: ");
        }
        catch (...) {
            ConsoleUtils::printError("Некорректное число. Повторите: ");
        }
    }
}

std::string Validator::inputPasswordMasked() {
    std::string p;
    char c;
    ConsoleUtils::showCursor();
    while ((c = _getch()) != 13) { // Enter
        if (c == KEY_ESC) {
            ConsoleUtils::hideCursor();
            throw OperationCancelled();
        }
        if (c == 8) { // Backspace
            if (!p.empty()) {
                std::cout << "\b \b";
                p.pop_back();
            }
        }
        else if (isprint(c)) {
            std::cout << "*";
            p += c;
        }
    }
    ConsoleUtils::hideCursor();
    std::cout << std::endl;
    return p;
}

PasswordStrength Validator::checkPasswordStrength(const std::string& pass) {
    if (pass.length() < 6) return PasswordStrength::WEAK;

    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpec = false;
    for (char c : pass) {
        if (islower(c)) hasLower = true;
        else if (isupper(c)) hasUpper = true;
        else if (isdigit(c)) hasDigit = true;
        else hasSpec = true;
    }

    if (pass.length() >= 8 && hasLower && hasUpper && hasDigit && hasSpec)
        return PasswordStrength::STRONG;

    if (hasDigit && (hasLower || hasUpper))
        return PasswordStrength::MEDIUM;

    return PasswordStrength::WEAK;
}

std::string Validator::getStrengthName(PasswordStrength s) {
    switch (s) {
    case PasswordStrength::WEAK: return "Слабый (нужно минимум 6 символов, буквы и цифры)";
    case PasswordStrength::MEDIUM: return "Средний";
    case PasswordStrength::STRONG: return "Сильный";
    default: return "";
    }
}