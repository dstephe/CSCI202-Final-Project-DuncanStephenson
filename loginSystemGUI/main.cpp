#include "loginSystem.h"

sqlite3* db;


int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Open the database connection
    int rc = sqlite3_open("user.db", &db); 

    // Check if the query execution was successful
    if (rc != SQLITE_OK) {
        // Display the error message as a popup window
        QMessageBox::critical(nullptr, "SQL Error", "Cannot open database: " + QString(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return -1;
    }

    loginSystem loginSystemObject(db);

    QWidget* loginWindow = new QWidget;
    QVBoxLayout* loginLayout = new QVBoxLayout(loginWindow);

    QLabel* loginLabel = new QLabel("Login Page");
    loginWindow->setWindowTitle("Login");
    loginWindow->setFixedSize(QSize(500, 200));
    loginLayout->addWidget(loginLabel);

    QLineEdit* usernameLineEdit = new QLineEdit;
    QLineEdit* passwordLineEdit = new QLineEdit;
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    loginLayout->addWidget(usernameLineEdit);
    loginLayout->addWidget(passwordLineEdit);

    QPushButton* loginButton = new QPushButton("Login");
    QPushButton* registerButton = new QPushButton("Register User");

    loginLayout->addWidget(loginButton);
    loginLayout->addWidget(registerButton);

    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        // Login button clicked
        std::string username = usernameLineEdit->text().toStdString();
        std::string password = passwordLineEdit->text().toStdString();
        //throws error is input is blank
        if (usernameLineEdit->text().isEmpty() || passwordLineEdit->text().isEmpty()) QMessageBox::critical(nullptr, "Login Error", "Username or password is blank!");
        else {
            // Perform login operation using the loginSystem object

            loginSystemObject.login(username, password);
        }
        });

    QObject::connect(registerButton, &QPushButton::clicked, [&]() {
        // Register User button clicked
        loginWindow->hide(); // Hide the login window
        loginSystemObject.signup(); // Pass the loginWindow as a parameter
          
            
            loginWindow->show();
            });

    loginWindow->setLayout(loginLayout); // Set the layout for the login window
    loginWindow->show(); // Display the login window

    return app.exec();
}





