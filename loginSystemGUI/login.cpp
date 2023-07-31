#include "loginSystem.h"

std::random_device rd;     //used once to initialise
std::mt19937 rng(rd());    // Random-number engine Mersenne-Twister

 QtWidgetsApp::QtWidgetsApp(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

QtWidgetsApp::~QtWidgetsApp()
{}


// Function to perform user login
void loginSystem::login(const std::string& username, const std::string& password) {
    queryResults.clear();

    //
    std::string selectQuery = "SELECT * FROM users WHERE username = '" + username + "';";
    char* errorMsg;
    int rc = sqlite3_exec(db, selectQuery.c_str(), staticCallback, &queryResults, &errorMsg);

    //Check for errors
    if (rc != SQLITE_OK) {
        // Display the error message as a popup window
        QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
        sqlite3_free(errorMsg);
    }

    // Check if the user exists
    if (queryResults.size() > 0) {
        //Check if the password is correct
        std::string storedPassword = queryResults[0][1];
        //Check if salt is correct
        std::string salt = queryResults[0][2];
        //Check if hashed password is correct
        std::string hashedPassword = pbkdf2(password, salt);

        std::cout << "Stored Password: " << storedPassword << std::endl;
        std::cout << "Salt: " << salt << std::endl;
        std::cout << "Hashed Password: " << hashedPassword << std::endl;

        // Check if the passwords match
        if (hashedPassword == storedPassword) {
            QMessageBox::information(nullptr, "Login Successful", "Login successful!");

            //If user is admin, access the admin panel
            if (username == "admin") adminLogin();

            //exit the program
            exit(0);
        }
        else {
            //Inform user information entered is incorrect
            QMessageBox::critical(nullptr, "Login Error", "Invalid username or password!");
        }

    }
}


                
 // Function to perform user signup
void loginSystem::signup() {
  
    queryResults.clear();
    char* errorMsg = nullptr;
    int rc = 0;
    std::string username;
    std::string password;
    QDialog signupPage; // Create a QDialog window for the signup page
    QVBoxLayout* signupLayout = new QVBoxLayout;
    QLabel usernameLabel("Username:", &signupPage);
    QLineEdit usernameInput(&signupPage);
    QLabel passwordLabel("Password:", &signupPage);
    QLineEdit passwordInput(&signupPage);
    passwordInput.setEchoMode(QLineEdit::Password);
    signupPage.setWindowTitle("Signup Page"); // Set the window title
    signupPage.setFixedSize(QSize(800, 600));

    // Create and layout widgets for the signup page UI (replace with your actual UI code)
    QLabel* label1 = new QLabel("Welcome to Signup Page!", &signupPage);
    QLabel* label2 = new QLabel("Please fill out the required information to create an account.", &signupPage);

    signupLayout->addWidget(label1);
    signupLayout->addWidget(label2);
    signupLayout->addWidget(&usernameLabel);
    signupLayout->addWidget(&usernameInput);
    signupLayout->addWidget(&passwordLabel);
    signupLayout->addWidget(&passwordInput);
    // Add signup button
    QPushButton* signupButton = new QPushButton("Signup", &signupPage);

    // Add generate password button
    QPushButton* generatePasswordButton = new QPushButton("Generate Password", &signupPage);

    signupLayout->addWidget(generatePasswordButton);
    signupLayout->addWidget(signupButton);
    // Set the layout for the signup page
    signupPage.setLayout(signupLayout);
    // Connect the signup button to a slot that performs the signup process
    connect(signupButton, &QPushButton::clicked, [&]() {
        username = usernameInput.text().toStdString();
        password = passwordInput.text().toStdString();
       
        //throws error is input is blank
        if (usernameInput.text().isEmpty()|| passwordInput.text().isEmpty()) QMessageBox::critical(nullptr, "Login Error", "Username or password is blank!");
        else {
            bool userFound = false;
            // Check if the user exists
            std::string selectQuery = "SELECT * FROM users;";

            rc = sqlite3_exec(db, selectQuery.c_str(), loginSystem::staticCallback, &queryResults, &errorMsg);
            for (size_t i = 0; i < queryResults.size(); i++) {
                if (queryResults[i][0] == username) {
                    userFound = true;
                    break;
                }
            }
            if (userFound) {
                // User already exists, display error message
                QMessageBox::critical(nullptr, "Signup Error", "Username already exists!");
            }
            else {
                // User does not exist, add user to the 'users' table    

                std::string newSalt = generate_salt();
                std::string newHashedPassword = pbkdf2(password, newSalt);
                std::string insertQuery = "INSERT INTO users (username, password, salt) VALUES ('";
                insertQuery += username;
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
                    // Display success message
                    QMessageBox::information(nullptr, "Signup Successful", "User was successfully signed up!");
                    signupPage.close();
                }
            }
        }
        });
    connect(generatePasswordButton, &QPushButton::clicked, [&]() {
        passwordGen();
        });
    signupPage.exec();
}


 
//Function to generate a password
void loginSystem::passwordGen() {
    //variable declaration
    int range = 0;
   
    std::string password;
    std::string lAlph = "abcdefghijklmnopqrstuvwxyz";
    std::string uAlph = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string numbers = "0123456789";
    std::string symbols = "!@#$%^&*()_+-=[]{}|;:,.<>/?";
    std::string chars;

    // Create a QDialog window for the password generator
    QDialog passwordGenPage;
    passwordGenPage.setWindowTitle("Password Generator");
    QVBoxLayout* passwordGenLayout = new QVBoxLayout;

    // Add length dropdown box
    QLabel lengthLabel("Password Length:", &passwordGenPage);
    QSpinBox lengthSpinBox(&passwordGenPage);
    lengthSpinBox.setMinimum(1);
    lengthSpinBox.setMaximum(50);

    // Add checkmark buttons for character options
    QCheckBox* lowercaseCheckbox = new QCheckBox("Include Lowercase letters", &passwordGenPage);
    QCheckBox* uppercaseCheckbox = new QCheckBox("Include Uppercase letters", &passwordGenPage);
    QCheckBox* numberCheckbox = new QCheckBox("Include Numbers", &passwordGenPage);
    QCheckBox* symbolsCheckbox = new QCheckBox("Include Symbols", &passwordGenPage);

    // Add generate password button
    QPushButton* generatePasswordButton = new QPushButton("Generate Password", &passwordGenPage);

    // Add widgets to layout
    passwordGenLayout->addWidget(&lengthLabel);
    passwordGenLayout->addWidget(&lengthSpinBox);
    passwordGenLayout->addWidget(lowercaseCheckbox);
    passwordGenLayout->addWidget(uppercaseCheckbox);
    passwordGenLayout->addWidget(numberCheckbox);
    passwordGenLayout->addWidget(symbolsCheckbox);
    passwordGenLayout->addWidget(generatePasswordButton);

    // Set the layout for the password generator page
    passwordGenPage.setLayout(passwordGenLayout);

    // Connect the generate password button to a slot that performs the password generation process
    connect(generatePasswordButton, &QPushButton::clicked, [&]() {
        // Get the selected password length
        int length = lengthSpinBox.value();

        // Get the selected character options
        std::string chars;
        if (lowercaseCheckbox->isChecked()) {
            chars += lAlph;
            range += lAlph.length();
        }
        if (uppercaseCheckbox->isChecked()) {
            chars += uAlph;
            range += uAlph.length();
        }
        if (numberCheckbox->isChecked()) {
            chars += numbers;
            range += numbers.length();
        }
        if (symbolsCheckbox->isChecked()) {
            chars += symbols;
            range += symbols.length();
        }
        if (range == 0) QMessageBox::critical(nullptr, "Invalid Selection", "You need to select at least one character type.");

        else {

            std::uniform_int_distribution<int> uni(0, range - 1);

            for (int i = 0; i < length; i++)
            {
                password += chars[uni(rng)];
            }

            // Output the generated password
        // Display the generated password and copy it to clipboard
            QMessageBox msgBox;
            msgBox.setWindowTitle("Password Generator");
            msgBox.setText("Generated Password");
            msgBox.setInformativeText(QString::fromStdString(password));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.button(QMessageBox::Ok)->setText("Copy to Clipboard");
            if (msgBox.exec() == QMessageBox::Ok) {
                QApplication::clipboard()->setText(QString::fromStdString(password));
                passwordGenPage.close();
            }
        }
         });
    // Show the password generator window
        passwordGenPage.exec();
        }
       


