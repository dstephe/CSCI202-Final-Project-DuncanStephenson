#include "loginSystem.h"
// Function with admin privileges
void loginSystem::adminLogin() {
    QDialog adminPanel; // Create a QDialog window for the admin panel
    QVBoxLayout* adminLayout = new QVBoxLayout;
    adminPanel.setWindowTitle("Admin Panel"); // Set the window title
    adminPanel.setFixedSize(QSize(800, 600));

    //variable declaration
    bool exit = false;
    int rc = 0;
    char* errorMsg = nullptr;

    std::string newPassword = "";
    std::string newSalt = "";
    std::string newHashedPassword = "";

    // Create and layout widgets for the admin panel UI (replace with your actual UI code)
    QLabel* label1 = new QLabel("Welcome, admin!", &adminPanel);
    QLabel* label2 = new QLabel("You have access to all the admin features. You can add, delete, and modify accounts.", &adminPanel);
    QLabel* label3 = new QLabel("If you are haven't already done so please change the admin account password from the default.", &adminPanel);

    adminLayout->addWidget(label1);
    adminLayout->addWidget(label2);
    adminLayout->addWidget(label3);

    QPushButton* addAccountButton = new QPushButton("Add Account", &adminPanel);
    QPushButton* deleteAccountButton = new QPushButton("Delete Account", &adminPanel);
    QPushButton* modifyAccountButton = new QPushButton("Modify Account", &adminPanel);
    QPushButton* printTableButton = new QPushButton("Print Table", &adminPanel);
    QPushButton* exitButton = new QPushButton("Exit", &adminPanel);

    QObject::connect(addAccountButton, &QPushButton::clicked, [this, rc, errorMsg, newPassword, newSalt, newHashedPassword] {
        addAccount(rc, errorMsg, newPassword, newSalt, newHashedPassword); // Call the addAccount function
        });

    QObject::connect(deleteAccountButton, &QPushButton::clicked, [this, rc, errorMsg] {
        deleteAccount(rc, errorMsg); // Call the printTable function
        });
    QObject::connect(modifyAccountButton, &QPushButton::clicked, [this, rc, errorMsg, newPassword, newSalt, newHashedPassword] {
        modifyAccount(rc, errorMsg, newPassword, newSalt, newHashedPassword); // Call the printTable function
        });
    QObject::connect(printTableButton, &QPushButton::clicked, [this] {
        printTable(); // Call the printTable function
        });
    QObject::connect(exitButton, &QPushButton::clicked, [&adminPanel]() {
        adminPanel.close(); // Close the admin panel window when the exit button is clicked
        });
    // Add the buttons to the layout

    adminLayout->addWidget(addAccountButton);
    adminLayout->addWidget(deleteAccountButton);
    adminLayout->addWidget(modifyAccountButton);
    adminLayout->addWidget(printTableButton);
    adminLayout->addWidget(exitButton);
    adminPanel.setLayout(adminLayout); // Set the layout for the admin panel

    adminPanel.exec(); // Open the admin panel window

}
void loginSystem::addAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword) {
    queryResults.clear();
    std::string newUsername = "";
    std::string insertQuery;

    // Create QDialog window for adding a new account
    QDialog addAccountDialog;
    addAccountDialog.setWindowTitle("Add Account");
    addAccountDialog.setFixedSize(QSize(400, 200));

    // Create and layout widgets for the add account UI
    QLabel* usernameLabel = new QLabel("Username:", &addAccountDialog);
    QLabel* passwordLabel = new QLabel("Password:", &addAccountDialog);
    QLineEdit* usernameInput = new QLineEdit(&addAccountDialog);
    QLineEdit* passwordInput = new QLineEdit(&addAccountDialog);
    // Add generate password button
    QPushButton* generatePasswordButton = new QPushButton("Generate Password", &addAccountDialog);
    QVBoxLayout* layout = new QVBoxLayout(&addAccountDialog);
    

    
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);
    layout->addWidget(generatePasswordButton);

    // Create Add User button for adding the user
    QPushButton* addUserButton = new QPushButton("Add User", &addAccountDialog);
    layout->addWidget(addUserButton);

    // Connect the addUserButton to the add user slot
    connect(addUserButton, &QPushButton::clicked, [&]() {
        // Get the username and password from the input fields
        newUsername = usernameInput->text().toStdString();
        newPassword = passwordInput->text().toStdString();

        // Check if the username already exists
        std::vector<std::vector<std::string>>::iterator it = std::find_if(queryResults.begin(), queryResults.end(),
            [&](const std::vector<std::string>& row) { return row[0] == newUsername; });

        if (it != queryResults.end()) {
            // User already exists, display error message
            QMessageBox::critical(nullptr, "Add User Error", "Username already exists!");
        }
        else {
            // User does not exist, add user to the 'users' table    

            newSalt = generate_salt();
            newHashedPassword = pbkdf2(newPassword, newSalt);
            insertQuery = "INSERT INTO users (username, password, salt) VALUES ('";
            insertQuery += newUsername;
            insertQuery += "', '" + newHashedPassword;
            insertQuery += "', '" + newSalt;
            insertQuery += "');";
            rc = sqlite3_exec(db, insertQuery.c_str(), staticCallback, 0, &errorMsg);
            if (rc != SQLITE_OK) {
                // Display the error message as a popup window
                QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
                sqlite3_free(errorMsg);
            }
            else {
                // User added successfully, display success message
                QMessageBox::information(nullptr, "Account added Successfully", "User added successfully!");
                // Clear the input fields
                usernameInput->clear();
                passwordInput->clear();
                addAccountDialog.close();
            }
        }
        });
    connect(generatePasswordButton, &QPushButton::clicked, [&]() {
        passwordGen();
        });

    // Display the add account dialog
    addAccountDialog.exec();
}
void loginSystem::deleteAccount(int rc, char* errorMsg) {
    queryResults.clear();
    std::string usernameToDelete = "";
    std::string deleteQuery;
    // Create a dialog window to input the username
    QDialog deleteAccountDialog;
    deleteAccountDialog.setWindowTitle("Delete Account");
    deleteAccountDialog.setFixedSize(QSize(400, 200));
    QVBoxLayout layout(&deleteAccountDialog);
    QLabel label("Username:", &deleteAccountDialog);
    QLineEdit input(&deleteAccountDialog);
    QPushButton deleteButton("Delete", &deleteAccountDialog);
    QPushButton cancelButton("Cancel", &deleteAccountDialog);

    layout.addWidget(&label);
    layout.addWidget(&input);
    layout.addWidget(&deleteButton);
    layout.addWidget(&cancelButton);

    QObject::connect(&deleteButton, &QPushButton::clicked, [&]() {
        usernameToDelete = input.text().toStdString();
        bool userFound = false;
        // Check if the user exists
        std::string selectQuery = "SELECT * FROM users;";

        rc = sqlite3_exec(db, selectQuery.c_str(), loginSystem::staticCallback, &queryResults, &errorMsg);
        for (size_t i = 0; i < queryResults.size(); i++) {
            if (queryResults[i][0] == usernameToDelete) {
                userFound = true;
                break;
            }
        }
        //Only move forward if user is found or isn't admin account
        if (userFound || (usernameToDelete == "admin")) {
            // Confirm deletion with the user
            QMessageBox confirmBox;
            confirmBox.setIcon(QMessageBox::Warning);
            confirmBox.setText("Are you sure you want to delete the user?");
            confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            confirmBox.setDefaultButton(QMessageBox::No);
            int response = confirmBox.exec();

            if (response == QMessageBox::Yes) {
                // Delete the user from the table
                deleteQuery = "DELETE FROM users WHERE username = '" + usernameToDelete + "';";
                rc = sqlite3_exec(db, deleteQuery.c_str(), staticCallback, 0, &errorMsg);

                if (rc != SQLITE_OK) {
                    // Display the error message as a popup window
                    QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
                    sqlite3_free(errorMsg);
                }
                else {
                    QMessageBox::information(nullptr, "User Deleted", "User deleted successfully!");
                }
            }
        }
        else {
            QMessageBox::critical(nullptr, "User Not Found", "User not found!");
        }

        // Close the dialog
        deleteAccountDialog.close();
        });

    QObject::connect(&cancelButton, &QPushButton::clicked, [&]() {
        // Close the dialog without performing any action
        deleteAccountDialog.close();
        });

    deleteAccountDialog.exec();
}
void loginSystem::modifyAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword) {
    queryResults.clear();
    std::string usernameToModify = "";
    std::string confirmPassword;
    std::string updateQuery;
    // Create a dialog window to input the username
    QDialog modifyAccountDialog;
    modifyAccountDialog.setWindowTitle("Modify Account");
    modifyAccountDialog.setFixedSize(QSize(400, 200));
    QVBoxLayout layout(&modifyAccountDialog);
    QLabel label("Username:", &modifyAccountDialog);
    QLineEdit input(&modifyAccountDialog);
    QPushButton modifyButton("Modify", &modifyAccountDialog);
    QPushButton cancelButton("Cancel", &modifyAccountDialog);

    layout.addWidget(&label);
    layout.addWidget(&input);
    layout.addWidget(&modifyButton);
    layout.addWidget(&cancelButton);

    QObject::connect(&modifyButton, &QPushButton::clicked, [&]() {
        usernameToModify = input.text().toStdString();
        bool userFound = false;

        // Check if the user exists
        std::string selectQuery = "SELECT * FROM users;";

        rc = sqlite3_exec(db, selectQuery.c_str(), loginSystem::staticCallback, &queryResults, &errorMsg);
        for (size_t i = 0; i < queryResults.size(); i++) {
            if (queryResults[i][0] == usernameToModify) {
                userFound = true;
                break;
            }
        }

        if (userFound) {
            // Create a dialog window to input the new password and confirm password
            QDialog modifyDialog;
            QVBoxLayout modifyLayout(&modifyDialog);
            QLabel newPasswordLabel("New Password:", &modifyDialog);
            QLineEdit newPasswordInput(&modifyDialog);
            QLabel confirmLabel("Confirm Password:", &modifyDialog);
            QLineEdit confirmInput(&modifyDialog);
            QPushButton confirmButton("Confirm", &modifyDialog);

            modifyLayout.addWidget(&newPasswordLabel);
            modifyLayout.addWidget(&newPasswordInput);
            modifyLayout.addWidget(&confirmLabel);
            modifyLayout.addWidget(&confirmInput);
            modifyLayout.addWidget(&confirmButton);

            QObject::connect(&confirmButton, &QPushButton::clicked, [&]() {
                newPassword = newPasswordInput.text().toStdString();
                confirmPassword = confirmInput.text().toStdString();

                if (newPassword == confirmPassword) {
                    // Modify the user's password in the table
                    newSalt = generate_salt();
                    newHashedPassword = pbkdf2(newPassword, newSalt);
                    updateQuery = "UPDATE users SET password = '" + newHashedPassword;
                    updateQuery += "', salt = '" + newSalt;
                    updateQuery += "' WHERE username = '" + usernameToModify + "';";
                    rc = sqlite3_exec(db, updateQuery.c_str(), staticCallback, 0, &errorMsg);

                    if (rc != SQLITE_OK) {
                        // Display the error message as a popup window
                        QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
                        sqlite3_free(errorMsg);
                    }
                    else {
                        QMessageBox::information(nullptr, "User Modified", "User modified successfully!");
                    }
                }
                else {
                    QMessageBox::critical(nullptr, "Password Mismatch", "New password and confirm password do not match!");
                }

                // Close the modify dialog
                modifyDialog.close();
                });

            modifyDialog.exec();

        }
        else {
            QMessageBox::critical(nullptr, "User Not Found", "User not found!");
        }

        // Close the dialog
        modifyAccountDialog.close();
        });

    QObject::connect(&cancelButton, &QPushButton::clicked, [&]() {
        // Close the dialog without performing any action
        modifyAccountDialog.close();
        });

    modifyAccountDialog.exec();
}