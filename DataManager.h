#pragma once
#include <unordered_map>
#include <vector>
#include "Models.h"

class DataManager {
private:
    std::unordered_map<std::string, Account*> accounts;
    std::vector<Question> questions;
    const std::string fileUsers = "users.txt";
    const std::string fileQuestions = "questions.txt";

    size_t hashPass(const std::string& p, const std::string& s);
    std::string genSalt();

    void loadUsers();
    void loadQuestions();

public:
    DataManager();
    ~DataManager();

    void registerUser(std::string login, std::string pass, bool isAdmin);
    Account* login(std::string login, std::string pass);
    bool userExists(std::string login);

    // Новые методы
    bool changeUserRole(std::string login, bool toAdmin);
    bool isLastAdmin(std::string loginToCheck);
    void updateUserPassword(std::string login, std::string newPass);

    std::vector<Question>& getQuestions();
    std::unordered_map<std::string, Account*>& getAccounts();

    void saveUsers();
    void saveQuestionsFile();
};