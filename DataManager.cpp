#define _CRT_SECURE_NO_WARNINGS
#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <iostream>

DataManager::DataManager() {
    srand(static_cast<unsigned>(time(0)));
    loadQuestions();
    loadUsers();

    if (accounts.find("admin") == accounts.end()) {
        registerUser("admin", "admin", true);
    }
    if (accounts.find("user") == accounts.end()) {
        registerUser("user", "user", false);
    }
}

DataManager::~DataManager() {
    saveUsers();
    for (auto& pair : accounts) delete pair.second;
}

size_t DataManager::hashPass(const std::string& p, const std::string& s) {
    std::hash<std::string> hasher;
    return hasher(p + s);
}

std::string DataManager::genSalt() {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string s;
    for (int i = 0; i < 16; ++i) s += chars[rand() % chars.size()];
    return s;
}

void DataManager::registerUser(std::string login, std::string pass, bool isAdmin) {
    std::string salt = genSalt();
    size_t h = hashPass(pass, salt);
    if (isAdmin)
        accounts[login] = new Admin(login, h, salt);
    else
        accounts[login] = new User(login, h, salt);
    saveUsers();
}

Account* DataManager::login(std::string login, std::string pass) {
    if (accounts.find(login) != accounts.end()) {
        Account* acc = accounts[login];
        if (hashPass(pass, acc->getSalt()) == acc->getHash()) {
            return acc;
        }
    }
    return nullptr;
}

bool DataManager::userExists(std::string login) {
    return accounts.find(login) != accounts.end();
}

//         ,                                           
bool DataManager::isLastAdmin(std::string loginToCheck) {
    int adminCount = 0;
    for (auto& p : accounts) {
        if (p.second->getRoleName() == "Admin") adminCount++;
    }

    //            1                ,                   
    if (adminCount == 1 && accounts[loginToCheck]->getRoleName() == "Admin") {
        return true;
    }
    return false;
}

//            (                    )
bool DataManager::changeUserRole(std::string login, bool toAdmin) {
    if (accounts.find(login) == accounts.end()) return false;

    Account* oldAcc = accounts[login];

    //                                            
    if (!toAdmin && isLastAdmin(login)) return false;

    //                      
    if ((toAdmin && oldAcc->getRoleName() == "Admin") || (!toAdmin && oldAcc->getRoleName() == "User")) {
        return true;
    }

    Account* newAcc;
    //                  (      ,     )
    if (toAdmin) {
        newAcc = new Admin(oldAcc->getLogin(), oldAcc->getHash(), oldAcc->getSalt());
    }
    else {
        newAcc = new User(oldAcc->getLogin(), oldAcc->getHash(), oldAcc->getSalt());
    }

    accounts[login] = newAcc;
    delete oldAcc;
    saveUsers();
    return true;
}

void DataManager::updateUserPassword(std::string login, std::string newPass) {
    if (accounts.find(login) != accounts.end()) {
        //                                       
        std::string newSalt = genSalt();
        size_t newHash = hashPass(newPass, newSalt);

        //   C++                             Account,                        protected/private             .
        //            DataManager                                               .
        //                                             -                                .

        Account* oldAcc = accounts[login];
        Account* newAcc;

        if (oldAcc->getRoleName() == "Admin") {
            newAcc = new Admin(login, newHash, newSalt);
        }
        else {
            User* oldUser = dynamic_cast<User*>(oldAcc);
            User* newUser = new User(login, newHash, newSalt);
            //                 ,          
            if (oldUser) {
                for (auto res : oldUser->getHistory()) newUser->addResult(res);
            }
            newAcc = newUser;
        }

        accounts[login] = newAcc;
        delete oldAcc;
        saveUsers();
    }
}

std::vector<Question>& DataManager::getQuestions() {
    return questions;
}

std::unordered_map<std::string, Account*>& DataManager::getAccounts() {
    return accounts;
}

void DataManager::saveUsers() {
    std::ofstream file(fileUsers);
    for (auto& pair : accounts) {
        file << pair.second->getRoleName() << "|"
            << pair.second->getLogin() << "|"
            << pair.second->getHash() << "|"
            << pair.second->getSalt();

        if (pair.second->getRoleName() == "User") {
            User* u = dynamic_cast<User*>(pair.second);
            for (const auto& res : u->getHistory()) {
                file << "|" << res.category << ";" << res.score << ";" << res.total << ";" << res.date;
            }
        }
        file << std::endl;
    }
}

void DataManager::loadUsers() {
    std::ifstream file(fileUsers);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        try {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> parts;
            while (std::getline(ss, segment, '|')) parts.push_back(segment);

            if (parts.size() >= 4) {
                std::string role = parts[0];
                std::string log = parts[1];
                size_t h = std::stoull(parts[2]);
                std::string s = parts[3];

                if (role == "Admin") {
                    accounts[log] = new Admin(log, h, s);
                }
                else {
                    User* u = new User(log, h, s);
                    for (size_t i = 4; i < parts.size(); ++i) {
                        std::stringstream ssRes(parts[i]);
                        std::string rSeg;
                        std::vector<std::string> rParts;
                        while (std::getline(ssRes, rSeg, ';')) rParts.push_back(rSeg);

                        if (rParts.size() == 4) {
                            TestResult tr;
                            tr.category = rParts[0];
                            tr.score = std::stoi(rParts[1]);
                            tr.total = std::stoi(rParts[2]);
                            tr.date = rParts[3];
                            u->addResult(tr);
                        }
                    }
                    accounts[log] = u;
                }
            }
        }
        catch (...) { continue; }
    }
}

void DataManager::loadQuestions() {
    questions.clear();
    std::ifstream file(fileQuestions);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

        try {
            std::stringstream ss(line);
            std::string seg;
            std::vector<std::string> p;
            while (std::getline(ss, seg, '|')) p.push_back(seg);

            if (p.size() >= 8) {
                Question q;
                q.id = std::stoi(p[0]);
                q.category = p[1];
                q.text = p[2];
                for (int i = 3; i <= 6; ++i) q.options.push_back(p[i]);
                q.correctIndex = std::stoi(p[7]);
                questions.push_back(q);
            }
        }
        catch (...) { continue; }
    }
}

void DataManager::saveQuestionsFile() {
    std::ofstream file(fileQuestions);
    for (const auto& q : questions) {
        file << q.id << "|" << q.category << "|" << q.text << "|";
        for (const auto& o : q.options) file << o << "|";
        file << q.correctIndex << std::endl;
    }
}