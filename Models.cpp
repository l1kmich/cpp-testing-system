#include "Models.h"

Account::Account(std::string l, size_t ph, std::string s) : login(l), passHash(ph), salt(s) {}
Account::~Account() {}
std::string Account::getLogin() const { return login; }
size_t Account::getHash() const { return passHash; }
std::string Account::getSalt() const { return salt; }

User::User(std::string l, size_t ph, std::string s, int level) : Account(l, ph, s), codeLevel(level) {}
std::string User::getRoleName() const { return "User"; }
void User::addResult(const TestResult& res) { history.push_back(res); }
const std::vector<TestResult>& User::getHistory() const { return history; }
int User::getLevel() const { return codeLevel; }
void User::setLevel(int l) { codeLevel = l; }
void User::incrementLevel() { if (codeLevel < 10) codeLevel++; }

Admin::Admin(std::string l, size_t ph, std::string s) : Account(l, ph, s) {}
std::string Admin::getRoleName() const { return "Admin"; }

int User::getTotalScore() const {
    int sum = 0;
    for (const auto& r : history) sum += r.score;
    return sum;
}
