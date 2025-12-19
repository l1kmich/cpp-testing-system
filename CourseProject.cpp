#define _CRT_SECURE_NO_WARNINGS
#include "App.h"
#include <iostream>
#include <windows.h>

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    try {
        App app;
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        system("pause");
    }
    return 0;
}