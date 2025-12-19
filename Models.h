#pragma once
#include <string>
#include <vector>

struct Question {
    int id;
    std::string category;
    std::string text;
    std::vector<std::string> options;
    int correctIndex; 
};

struct TestResult {
    std::string category;
    int score;
    int total;
    std::string date;
};

class Account {
protected:
    std::string login;
    size_t passHash;
    std::string salt;
public:
    Account(std::string l, size_t ph, std::string s);
    virtual ~Account();
    std::string getLogin() const;
    size_t getHash() const;
    std::string getSalt() const;
    virtual std::string getRoleName() const = 0;
};

class User : public Account {
    std::vector<TestResult> history;
    int codeLevel; 
public:
    User(std::string l, size_t ph, std::string s, int level = 1);
    std::string getRoleName() const override;
    void addResult(const TestResult& res);
    const std::vector<TestResult>& getHistory() const;
    int getLevel() const;
    void setLevel(int l);
    void incrementLevel();
    int getTotalScore() const;
};

class Admin : public Account {
public:
    Admin(std::string l, size_t ph, std::string s);
    std::string getRoleName() const override;
};