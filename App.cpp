#define _CRT_SECURE_NO_WARNINGS
#include "App.h"
#include "Validator.h"
#include "ConsoleUtils.h"
#include <algorithm>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <chrono>
#include <sstream>

App::App() : currentAccount(nullptr) {}

std::string App::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
    return ss.str();
}

void App::run() {
    ConsoleUtils::hideCursor();
   
    while (true) {
        try {
            std::vector<std::string> opts = { "Войти", "Регистрация", "Выход" };
            int ch = ConsoleUtils::showMenu("СИСТЕМА ТЕСТИРОВАНИЯ C++", opts);
            if (ch == 0) loginMenu();
            else if (ch == 1) registerMenu();
            else exit(0);
        }
        catch (const OperationCancelled&) {
        }
    }
}

void App::mainMenu() {
    // Логика в run()
}

void App::loginMenu() {
    try {
        ConsoleUtils::clear();
        ConsoleUtils::printHeader("АВТОРИЗАЦИЯ");
        std::cout << "Логин: ";
        std::string l = Validator::getStringInput();
        std::cout << "Пароль: ";
        std::string p = Validator::inputPasswordMasked();

        currentAccount = db.login(Validator::toLowerCase(l), p); // Использование нижнего регистра
        if (currentAccount) {
            ConsoleUtils::printSuccess("Успешный вход!");
            ConsoleUtils::pause();

            if (currentAccount->getRoleName() == "Admin") adminLoop();
            else userLoop();

            currentAccount = nullptr;
        }
        else {
            ConsoleUtils::printError("Неверные данные.");
            ConsoleUtils::pause();
        }
    }
    catch (const OperationCancelled&) {
        return;
    }
}

void App::registerMenu() {
    try {
        ConsoleUtils::clear();
        ConsoleUtils::printHeader("РЕГИСТРАЦИЯ");

        // 1. Ввод и нормализация логина
        std::cout << "Логин (a-z, 0-9, пробел): ";
        std::string l = Validator::getStringInput();
        std::string lowerL = Validator::toLowerCase(l);

        if (l != lowerL) {
            ConsoleUtils::printError("Обнаружены заглавные буквы. Имя будет использовано в нижнем регистре: " + lowerL);
            l = lowerL;
        }

        if (!Validator::isValidLogin(l)) {
            ConsoleUtils::printError("Ошибка: Недопустимые символы в логине!");
            ConsoleUtils::pause(); return;
        }

        // 2. Проверка уникальности (все аккаунты хранятся в нижнем регистре)
        if (db.userExists(l)) {
            ConsoleUtils::printError("Ошибка: Пользователь с таким именем (независимо от регистра) уже существует!");
            ConsoleUtils::pause(); return;
        }

        // 3. Проверка сложности пароля
        std::string p;
        while (true) {
            std::cout << "Пароль: ";
            p = Validator::inputPasswordMasked();

            PasswordStrength strength = Validator::checkPasswordStrength(p);

            if (strength == PasswordStrength::WEAK) {
                ConsoleUtils::printError("Пароль слишком простой! " + Validator::getStrengthName(strength) + ". Придумайте новый.");
                // Цикл повторяется
            }
            else {
                ConsoleUtils::printSuccess("Сложность пароля: " + Validator::getStrengthName(strength));
                break;
            }
        }

        db.registerUser(l, p, false);
        ConsoleUtils::printSuccess("Аккаунт создан.");
        ConsoleUtils::pause();
    }
    catch (const OperationCancelled&) {
        return; // Возврат в главное меню
    }
}

// === ADMIN ===

void App::adminLoop() {
    while (true) {
        try {
            std::vector<std::string> opts = {
                "Управление вопросами",
                "Управление пользователями",
                "Таблица лидеров",
                "Назад",
                "Выход"
            };
            int ch = ConsoleUtils::showMenu("АДМИН ПАНЕЛЬ", opts);
            if (ch == 0) manageQuestions();
            else if (ch == 1) manageUsers();
            else if (ch == 2) showLeaderboard();
            else if (ch == 3) return;
            else              exit(0);
        }
        catch (const OperationCancelled&) {
            return; // Выход из админ-панели
        }
    }
}

void App::manageUsers() {
    while (true) {
        try {
            std::vector<std::string> opts = { "Показать всех", "Сортировка", "Фильтр", "Поиск", "Изменить роль", "Назад" };
            int ch = ConsoleUtils::showMenu("УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ", opts);

            auto& accs = db.getAccounts();
            std::vector<Account*> vec;
            for (auto& p : accs) vec.push_back(p.second);

            if (ch == 0) {
                printUsersTable(vec);
            }
            else if (ch == 1) { // Сортировка
                int sortType = ConsoleUtils::showMenu("Сортировать по:", { "Логину", "Роли" });
                if (sortType == 0) std::sort(vec.begin(), vec.end(), [](Account* a, Account* b) { return a->getLogin() < b->getLogin(); });
                else std::sort(vec.begin(), vec.end(), [](Account* a, Account* b) { return a->getRoleName() < b->getRoleName(); });
                printUsersTable(vec);
            }
            else if (ch == 2) { // Фильтр
                int fType = ConsoleUtils::showMenu("Показать только:", { "Админов", "Пользователей" });
                std::string target = (fType == 0) ? "Admin" : "User";
                std::vector<Account*> filtered;
                std::copy_if(vec.begin(), vec.end(), std::back_inserter(filtered), [&](Account* a) { return a->getRoleName() == target; });
                printUsersTable(filtered);
            }
            else if (ch == 3) { // Поиск
                ConsoleUtils::clear();
                std::cout << "Введите логин: ";
                std::string term = Validator::getStringInput();
                std::string lowerTerm = Validator::toLowerCase(term); // Поиск по нижнему регистру

                auto it = std::find_if(vec.begin(), vec.end(), [&](Account* a) { return a->getLogin() == lowerTerm; });
                if (it != vec.end()) {
                    ConsoleUtils::printSuccess("Найдено: " + (*it)->getLogin() + " [" + (*it)->getRoleName() + "]");
                }
                else ConsoleUtils::printError("Не найдено.");
                ConsoleUtils::pause();
            }
            else if (ch == 4) { // Изменить роль
                ConsoleUtils::clear();
                std::cout << "Введите логин пользователя: ";
                std::string targetLogin = Validator::getStringInput();
                std::string lowerTargetLogin = Validator::toLowerCase(targetLogin);

                if (!db.userExists(lowerTargetLogin)) {
                    ConsoleUtils::printError("Пользователь не найден.");
                }
                else {
                    int roleCh = ConsoleUtils::showMenu("Выберите новую роль:", { "Пользователь", "Администратор" });
                    bool toAdmin = (roleCh == 1);

                    if (!toAdmin && db.isLastAdmin(lowerTargetLogin)) {
                        ConsoleUtils::printError("Ошибка! Невозможно понизить в правах единственного администратора.");
                    }
                    else if (db.changeUserRole(lowerTargetLogin, toAdmin)) {
                        ConsoleUtils::printSuccess("Роль изменена успешно. Пользователю необходимо перелогиниться.");
                        if (lowerTargetLogin == currentAccount->getLogin()) {
                            ConsoleUtils::printError("Ваша роль изменилась. Вы будете выведены в главное меню.");
                            ConsoleUtils::pause();
                            throw OperationCancelled(); // Выход из цикла админа
                        }
                    }
                    else {
                        ConsoleUtils::printError("Ошибка при изменении роли. Роль, возможно, уже соответствует выбранной.");
                    }
                }
                ConsoleUtils::pause();
            }
            else return;
        }
        catch (const OperationCancelled&) {
            return;
        }
    }
}

void App::printUsersTable(const std::vector<Account*>& vec) {
    ConsoleUtils::clear();
    std::cout << std::left << std::setw(20) << "Логин" << std::setw(10) << "Роль" << std::endl;
    std::cout << "------------------------------\n";
    for (auto* a : vec) {
        std::cout << std::left << std::setw(20) << a->getLogin() << std::setw(10) << a->getRoleName() << std::endl;
    }
    ConsoleUtils::pause();
}

void App::manageQuestions() {
    while (true) {
        try {
            std::vector<std::string> opts = { "Добавить вопрос", "Удалить вопрос", "Просмотр", "Назад" };
            int ch = ConsoleUtils::showMenu("УПРАВЛЕНИЕ ВОПРОСАМИ", opts);
            if (ch == 0) {
                ConsoleUtils::clear();
                Question q;
                auto& list = db.getQuestions();
                q.id = list.empty() ? 1 : list.back().id + 1;
                std::cout << "Категория: "; q.category = Validator::getStringInput();
                std::cout << "Текст: "; q.text = Validator::getStringInput();
                for (int i = 0; i < 4; i++) { std::cout << "Вариант " << i + 1 << ": "; q.options.push_back(Validator::getStringInput()); }
                std::cout << "Верный (1-4): "; q.correctIndex = Validator::getIntInput(1, 4);
                db.getQuestions().push_back(q);
                db.saveQuestionsFile();
                ConsoleUtils::printSuccess("Добавлено!");
                ConsoleUtils::pause();
            }
            else if (ch == 1) {
                ConsoleUtils::clear();
                std::cout << "Введите ID для удаления: ";
                int id = Validator::getIntInput(1, 10000);
                auto& list = db.getQuestions();
                auto it = std::remove_if(list.begin(), list.end(), [id](const Question& q) { return q.id == id; });
                if (it != list.end()) {
                    list.erase(it, list.end());
                    db.saveQuestionsFile();
                    ConsoleUtils::printSuccess("Удалено.");
                }
                else ConsoleUtils::printError("Не найдено.");
                ConsoleUtils::pause();
            }
            else if (ch == 2) {
                ConsoleUtils::clear();
                for (const auto& q : db.getQuestions()) {
                    std::cout << "[" << q.id << "] (" << q.category << ") " << q.text << std::endl;
                }
                ConsoleUtils::pause();
            }
            else return;
        }
        catch (const OperationCancelled&) {
            return;
        }
    }
}

void App::showLeaderboard() {
    ConsoleUtils::clear();
    ConsoleUtils::printHeader("ТОП 25 ПОЛЬЗОВАТЕЛЕЙ");

    auto& accs = db.getAccounts();
    std::vector<User*> students;

    for (auto& p : accs) {
        if (p.second->getRoleName() == "User") {
            students.push_back(dynamic_cast<User*>(p.second));
        }
    }

    std::sort(students.begin(), students.end(), [](User* a, User* b) {
        return a->getTotalScore() > b->getTotalScore();
        });

    int limit = std::min((int)students.size(), 25);
    std::cout << std::left << std::setw(5) << "#" << std::setw(20) << "Логин" << std::setw(10) << "Баллы" << std::endl;
    std::cout << "-------------------------------------\n";
    for (int i = 0; i < limit; ++i) {
        std::cout << std::left << std::setw(5) << (i + 1) << std::setw(20) << students[i]->getLogin()
            << std::setw(10) << students[i]->getTotalScore() << std::endl;
    }
    ConsoleUtils::pause();
}


// === USER ===

void App::userLoop() {
    while (true) {
        try {
            std::vector<std::string> opts = {
                "Стандартный тест",
                "Блиц (Выживание)",
                "Тест по категории",
                "Моя статистика",
                "Таблица лидеров",
                "Анализ кода",
                "Сменить пароль",
                "Назад",
                "Выход"
            };
            int ch = ConsoleUtils::showMenu("КАБИНЕТ: " + currentAccount->getLogin(), opts);

            if (ch == 3) {
                User* u = dynamic_cast<User*>(currentAccount);
                ConsoleUtils::clear();
                ConsoleUtils::printHeader("ИСТОРИЯ");
                if (u) {
                    for (const auto& res : u->getHistory()) {
                        std::cout << "Дата: " << res.date << " | " << std::setw(10) << res.category
                            << " | " << res.score << "/" << res.total << std::endl;
                    }
                }
                ConsoleUtils::pause();
            }
            else if (ch == 4) showLeaderboard();
            else if (ch == 5) runCodeAnalysis();
            else if (ch == 6) editProfile();
            else if (ch == 7) return;
            else if (ch == 8) exit(0);
            else runTest(ch);
        }
        catch (const OperationCancelled&) {
            return; // Выход из кабинета
        }
    }
}

void App::editProfile() {
    try {
        ConsoleUtils::clear();
        ConsoleUtils::printHeader("СМЕНА ПАРОЛЯ");

        std::string newPass;
        while (true) {
            std::cout << "Введите новый пароль: ";
            newPass = Validator::inputPasswordMasked();

            PasswordStrength s = Validator::checkPasswordStrength(newPass);
            if (s == PasswordStrength::WEAK) {
                ConsoleUtils::printError("Пароль слишком простой! " + Validator::getStrengthName(s) + ". Придумайте другой.");
                // Цикл продолжается, пока не будет введен надежный пароль
            }
            else {
                ConsoleUtils::printSuccess("Сложность пароля: " + Validator::getStrengthName(s));
                break;
            }
        }

        db.updateUserPassword(currentAccount->getLogin(), newPass);
        ConsoleUtils::printSuccess("Пароль изменен! Для завершения обновления, необходимо снова авторизоваться.");
        ConsoleUtils::pause();

        // Принудительный выход из userLoop
        throw OperationCancelled();
    }
    catch (const OperationCancelled&) {
        // Пробрасываем исключение дальше, чтобы выйти из userLoop
        throw;
    }
}

void App::runTest(int mode) {
    try {
        auto questions = db.getQuestions();
        if (questions.empty()) {
            ConsoleUtils::printError("Нет вопросов!");
            ConsoleUtils::pause(); return;
        }

        std::string testCategory = (mode == 1 ? "Выживание" : "Стандарт");

        // Фильтр по категории (режим 2)
        if (mode == 2) {
            std::set<std::string> cats;
            for (const auto& q : questions) cats.insert(q.category);
            std::vector<std::string> catVec(cats.begin(), cats.end());

            if (catVec.empty()) { ConsoleUtils::printError("Категории не найдены."); ConsoleUtils::pause(); return; }

            // Вопрос: "ВЫБЕРИТЕ КАТЕГОРИЮ"
            int cChoice = ConsoleUtils::showMenu("ВЫБЕРИТЕ КАТЕГОРИЮ", catVec);
            testCategory = catVec[cChoice];

            std::vector<Question> filtered;
            std::copy_if(questions.begin(), questions.end(), std::back_inserter(filtered),
                [&](const Question& q) { return q.category == testCategory; });
            questions = filtered;
        }

        if (questions.empty()) {
            ConsoleUtils::printError("В этой категории нет вопросов.");
            ConsoleUtils::pause(); return;
        }

        // Выбор режима отображения (сразу или потом)
        std::vector<std::string> feedbackOpts = { "Показывать ошибки сразу", "Результат в конце" };
        int feedbackMode = ConsoleUtils::showMenu("РЕЖИМ ОТОБРАЖЕНИЯ", feedbackOpts);

        ConsoleUtils::clear();
        ConsoleUtils::printHeader("НАСТРОЙКИ");
        std::cout << "Доступно вопросов: " << questions.size() << std::endl;
        std::cout << "Сколько вопросов задать? ";
        int count = Validator::getIntInput(1, (int)questions.size());

        // Перемешиваем
        std::random_shuffle(questions.begin(), questions.end());
        questions.resize(count);

        int score = 0;
        bool survivalFailed = false;
        int questionsAttempted = 0;

        for (int i = 0; i < count; ++i) {
            if (mode == 1 && survivalFailed) break; // Выживание

            questionsAttempted++;

            // Исправленное отображение: Вопрос + Варианты в одном меню
            std::string qTitle = "ВОПРОС " + std::to_string(i + 1) + " [" + questions[i].category + "]\n\n" + questions[i].text;
            int ans = ConsoleUtils::showMenu(qTitle, questions[i].options); // Текст вопроса отображается как заголовок

            bool correct = (ans + 1 == questions[i].correctIndex);

            if (correct) {
                score++;
                if (feedbackMode == 0) {
                    ConsoleUtils::printSuccess("Правильно!");
                    ConsoleUtils::pause();
                }
            }
            else {
                if (mode == 1) survivalFailed = true;
                if (feedbackMode == 0) {
                    ConsoleUtils::printError("Ошибка! Правильный ответ: " + questions[i].options[questions[i].correctIndex - 1]);
                    ConsoleUtils::pause();
                }
            }
        }

        ConsoleUtils::clear();
        ConsoleUtils::printHeader("РЕЗУЛЬТАТ ТЕСТА");
        //int totalQ = (mode == 1) ? questionsAttempted : count;
        int totalQ = count;
        std::cout << "Вы набрали: " << score << " из " << totalQ << std::endl;

        // Сохранение
        User* u = dynamic_cast<User*>(currentAccount);
        if (u) {
            TestResult tr;
            tr.category = testCategory;
            tr.score = score;
            tr.total = totalQ;
            tr.date = getCurrentDateTime();
            u->addResult(tr);
            db.saveUsers();
        }
        ConsoleUtils::pause();
    }
    catch (const OperationCancelled&) {
        return; // Возврат в меню пользователя
    }
}

void App::runCodeAnalysis() {
    User* u = dynamic_cast<User*>(currentAccount);
    if (!u) return;

    while (u->getLevel() <= 10) {
        CodeTask t = loadTask(u->getLevel());
        if (t.options.empty()) break;

        ConsoleUtils::clear();
        ConsoleUtils::printHeader("УРОВЕНЬ " + std::to_string(u->getLevel()));
        //ConsoleUtils::setColor(GREEN);
        ConsoleUtils::setColor(WHITE);

        int ch = ConsoleUtils::showMenuRunCodeAnalysis("Что выведет этот код?", t.options, t.code);
        if (ch + 1 == t.correct) {
            ConsoleUtils::printSuccess("Верно!");
            u->incrementLevel();
            db.saveUsers();
            ConsoleUtils::pause();
        }
        else {
            ConsoleUtils::printError("Неверно. Попробуйте снова.");
            ConsoleUtils::pause();
            break;
        }
    }
}

CodeTask App::loadTask(int lvl) {
   // std::ifstream f("tasks/level" + std::to_string(lvl) + ".txt");
    std::string path = "tasks/level" + std::to_string(lvl) + ".txt";
    std::ifstream f(path);
    CodeTask t; t.correct = 0; t.code = "";

    if (!f.is_open()) {
         ConsoleUtils::printError("Файл не найден: " + path); 
        return t;
    }

    std::string line, m = "code";
    while (std::getline(f, line)) {
        if (line == "---") { m = "opts"; continue; }
        if (line == "===") { m = "ans"; continue; }
        if (m == "code") {
            t.code += line + "\n";
            std::cout << "DEBUG: " << line << std::endl;
        }
        else if (m == "opts") t.options.push_back(line);
        else if (m == "ans") {
            try { t.correct = std::stoi(line); }
            catch (...) { t.correct = 0; }
        }
    }
    return t;
}