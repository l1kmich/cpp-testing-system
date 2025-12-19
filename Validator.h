#pragma once
#include <string>
#include <exception>

// Исключение для обработки нажатия ESC
class OperationCancelled : public std::exception {
public:
    const char* what() const noexcept override {
        return "Operation cancelled by user";
    }
};

enum class PasswordStrength {
    WEAK,   // < 6 символов
    MEDIUM, // >= 6, есть буквы и цифры
    STRONG  // >= 8, есть большие, маленькие, цифры и спецсимволы
};

class Validator {
public:
    static std::string toLowerCase(std::string str);
    static bool isValidLogin(const std::string& login);

    // Эти методы теперь могут выбросить OperationCancelled
    static std::string getStringInput();
    static int getIntInput(int min, int max);
    static std::string inputPasswordMasked();

    static PasswordStrength checkPasswordStrength(const std::string& pass);
    static std::string getStrengthName(PasswordStrength s);
};