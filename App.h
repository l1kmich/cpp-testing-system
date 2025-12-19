#pragma once
#include "DataManager.h"

struct CodeTask {
    std::string code;
    std::vector<std::string> options;
    int correct;
};

class App {
private:
    DataManager db;
    Account* currentAccount;

    void mainMenu();
    void loginMenu();
    void registerMenu();

    void adminLoop();
    void manageUsers();
    void printUsersTable(const std::vector<Account*>& vec);
    void manageQuestions();
    void showLeaderboard();
    void editProfile();

    void userLoop();
    void runTest(int mode); // 0-Standart, 1-Survival, 2-Category
    std::string getCurrentDateTime();

    void runCodeAnalysis();
    CodeTask loadTask(int level);

public:
    App();
    void run();
};