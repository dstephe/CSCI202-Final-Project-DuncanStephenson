#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED

#include <iostream>
#include <stdio.h>
#include <sqlite3.h>
#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

extern sqlite3* db;

class loginSystem {
public:
    loginSystem(sqlite3* database);
    ~loginSystem();
    void login(const std::string& username, const std::string& password);
    void signup();
    void printTable();
    void passwordGen();
    static int staticCallback(void* queryResultsPtr, int argc, char** argv, char** azColName);

private:
    std::vector<std::vector<std::string>> queryResults;

    std::string generate_salt();
    std::string pbkdf2(const std::string& password, const std::string& salt);
    void createTable();
    void adminLogin();
    void addAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword);
    void deleteAccount(int rc, char* errorMsg);
    void modifyAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword);

    sqlite3* db;
};





#endif // FUNCTIONS_HPP_INCLUDED
