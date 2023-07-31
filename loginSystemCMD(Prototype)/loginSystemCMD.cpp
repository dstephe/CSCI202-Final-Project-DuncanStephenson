#include <iostream>
#include <stdio.h>
#include <sqlite3.h>
#include "functions.hpp"

sqlite3* db;

void menu(loginSystem& loginSystemObject) {
    while (true) {
        std::string username;
        std::string password;
        loginSystemObject.printTable();
        std::cout << "Login or Signup" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Signup" << std::endl;
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Ignore the newline character left in the input buffer

        if (choice == 1) {
            std::cout << "Enter username: ";
            std::cin >> username;

            std::cout << "Enter password: ";
            std::cin >> password;

            loginSystemObject.login(username, password);

        }
        else if (choice == 2) {
            std::cout << "Did you want to generate a password? (y/n)" << std::endl;
            char choice;
            std::cin >> choice;
            if (choice == 'y') {
                loginSystemObject.passwordGen();
            }
            loginSystemObject.signup();

        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    // Open the database connection
    int rc = sqlite3_open("user.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Create the users table object and insert a sample row if not exists
    loginSystem loginSystemObject(db);

    // Call the menu function
    menu(loginSystemObject);

    return 0;
}