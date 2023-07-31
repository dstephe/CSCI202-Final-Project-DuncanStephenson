#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApp.h"
#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
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



class QtWidgetsApp : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApp(QWidget* parent = nullptr);
    ~QtWidgetsApp();

private:
    Ui::QtWidgetsAppClass ui;
};

class loginSystem : public QWidget {
    Q_OBJECT
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

    QLineEdit* usernameLineEdit;
    QLineEdit* passwordLineEdit;
    QPushButton* loginButton;
    QPushButton* signupButton ;
    QPushButton* passwordGenButton;

    //admin buttons
    QPushButton* addAccountButton = new QPushButton("Add Account");
    QPushButton* deleteAccountButton = new QPushButton("Delete Account");
    QPushButton* modifyAccountButton = new QPushButton("Modify Account");
    QPushButton* printTableButton = new QPushButton("Print User Data Table");

    sqlite3* db;
};
