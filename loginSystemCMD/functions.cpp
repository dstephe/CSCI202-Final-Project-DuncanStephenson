#include "functions.hpp"

std::random_device rd;     //used once to initialise
std::mt19937 rng(rd());    // Random-number engine Mersenne-Twister

// Constructor for the loginSystem class
loginSystem::loginSystem(sqlite3* database) {
    db = database;
    createTable();   // Create the 'users' table
}

// Destructor for the loginSystem class
loginSystem::~loginSystem() {
    // Cleanup resources
    sqlite3_close(db);
}

// Update the staticCallback function definition
int loginSystem::staticCallback(void* queryResultsPtr, int argc, char** argv, char** azColName) {
    // Cast the queryResultsPtr back to the original vector type
    std::vector<std::vector<std::string>>* queryResults = static_cast<std::vector<std::vector<std::string>>*>(queryResultsPtr);

    // Create a temporary vector to store the row data
    std::vector<std::string> row;

    // Iterate over the columns of the current row
    for (int i = 0; i < argc; i++) {
        // Add the column value to the current row vector
        row.push_back(argv[i]);
    }

    // Add the current row to the queryResults vector
    queryResults->push_back(row);

    return 0;
}

// Function to generate a salt value
std::string loginSystem::generate_salt() {
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    std::stringstream ss;
    for (int i = 0; i < sizeof(salt); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt[i]);
    }

    return ss.str();
}

// Function to perform PBKDF2 hashing
std::string loginSystem::pbkdf2(const std::string& password, const std::string& salt) {
    //variable declaration
    const EVP_MD* md = EVP_sha256();
    const unsigned int iterations = 10000;
    const unsigned int key_length = 32;
    unsigned char key[key_length];

    // Calculate the PBKDF2 hash
    PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(), iterations, md, key_length, key);

    // Convert the key to a hexadecimal string
    std::stringstream ss;
    for (int i = 0; i < key_length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(key[i]);
    }

    return ss.str();
}


// Function to create the 'users' table
void loginSystem::createTable() {
    char* errorMsg;

    // Create the 'users' table if it doesn't exist
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS users (" \
        "username TEXT PRIMARY KEY," \
        "password TEXT NOT NULL," \
        "salt TEXT NOT NULL);";

    // Execute the query
    int rc = sqlite3_exec(db, createTableQuery.c_str(), staticCallback, 0, &errorMsg);

    //Check for errors
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }

    //Generate a random salt value
    std::string salt = generate_salt();

    //Insert values into the 'users' table
    std::string insertQuery = "INSERT OR IGNORE INTO users (username, password, salt) VALUES ('admin', '" + pbkdf2("password", salt) + "', '" + salt + "');";

    // Execute the query
    rc = sqlite3_exec(db, insertQuery.c_str(), staticCallback, 0, &errorMsg);

    //Check for errors
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
}

// Function to perform user login
void loginSystem::login(const std::string& username, const std::string& password) {
    queryResults.clear();

    //
    std::string selectQuery = "SELECT * FROM users WHERE username = '" + username + "';";
    char* errorMsg;
    int rc = sqlite3_exec(db, selectQuery.c_str(), staticCallback, &queryResults, &errorMsg);

    //Check for errors
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
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
            std::cout << "Login successful!\n";

            //If user is admin, access the admin panel
            if (username == "admin") adminLogin();

            //exit the program
            exit(0);
        }
        else {
            //Inform user infomation entered is incorrect
            std::cout << "Invalid username or password!\n";
        }

    }
}

// Function with admin privileges
void loginSystem::adminLogin() {
    //variable declaration
    bool exit;
    int rc = 0;
    char* errorMsg = nullptr;

    std::string newPassword = "";
    std::string newSalt = "";
    std::string newHashedPassword = "";


    std::cout << "Welcome, admin!\n";
    std::cout << "You have access to all the admin features.\n";
    std::cout << "You can add, delete, and modify accounts.\n";
    std::cout << "If you are haven't already done so please change the admin account password from the default.\n";
    // Loop to display menu and perform actions
    do {
        exit = false;
        std::cout << "1. Add account\n";
        std::cout << "2. Delete account\n";
        std::cout << "3. Modify account\n";
        std::cout << "4. Print out the user data table\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        int choice;
        std::cin >> choice;
        switch (choice) {
            // Add account
        case 1:
            addAccount(rc, errorMsg, newPassword, newSalt, newHashedPassword);
            break;
            // Delete account
        case 2:
            deleteAccount(rc, errorMsg);
            break;
            // Modify account
        case 3:
            modifyAccount(rc, errorMsg, newPassword, newSalt, newHashedPassword);
            break;
            // Print out the user data table
        case 4:

            printTable();
            break;
            // Exit
        case 5:
            std::cout << "Exiting...\n";
            exit = true;
            break;
        }
    } while (!exit);
}

void loginSystem::addAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword) {
    std::string newUsername = "";
    std::string insertQuery;

    std::cout << "Enter the username of the new account: ";
    newUsername = "";
    std::cin >> newUsername;
    std::cout << "Enter the password of the new account: ";
    std::cin >> newPassword;
    newSalt = generate_salt();
    newHashedPassword = pbkdf2(newPassword, newSalt);
    insertQuery = "INSERT INTO users (username, password, salt) VALUES ('";
    insertQuery += newUsername;
    insertQuery += "', '" + newHashedPassword;
    insertQuery += "', '" + newSalt;
    insertQuery += "');";
    rc = sqlite3_exec(db, insertQuery.c_str(), staticCallback, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    std::cout << "Account added successfully!\n";
}
void loginSystem::deleteAccount(int rc, char* errorMsg) {
    std::string usernameToDelete = "";
    std::string deleteQuery;
    std::cout << "Enter the username of the account to delete: ";
    std::cin >> usernameToDelete;
    deleteQuery = "DELETE FROM users WHERE username = '" + usernameToDelete + "';";
    rc = sqlite3_exec(db, deleteQuery.c_str(), staticCallback, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    std::cout << "Account deleted successfully!\n";
}
void loginSystem::modifyAccount(int rc, char* errorMsg, std::string newPassword, std::string newSalt, std::string newHashedPassword) {
    std::string usernameToModify = "";
    std::string updateQuery;
    std::cout << "Enter the username of the account to modify: ";
    usernameToModify = "";
    std::cin >> usernameToModify;
    std::cout << "Enter the new password of the account: ";
    newPassword;
    std::cin >> newPassword;
    newSalt = generate_salt();
    newHashedPassword = pbkdf2(newPassword, newSalt);
    updateQuery = "UPDATE users SET password = '" + newHashedPassword;
    updateQuery += "', salt = '" + newSalt;
    updateQuery += "' WHERE username = '" + usernameToModify + "';";
    rc = sqlite3_exec(db, updateQuery.c_str(), staticCallback, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    std::cout << "Account modified successfully!\n";
}
// Function to signup an account
void loginSystem::signup() {
    queryResults.clear();

    std::string username;
    std::string password;

    std::cout << "Enter username: ";
    std::cin >> username;

    std::cout << "Enter password: ";
    std::cin >> password;

    std::string salt = generate_salt();
    std::string hashedPassword = pbkdf2(password, salt);

    std::string insertQuery = "INSERT INTO users (username, password, salt) VALUES ('" + username + "', '" + hashedPassword + "', '" + salt + "');";
    char* errorMsg;
    int rc = sqlite3_exec(db, insertQuery.c_str(), staticCallback, 0, &errorMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
}

//Function to generate a password
void loginSystem::passwordGen() {
    //variable declaration
    int range = 0;
    int length;
    char lCase;
    char uCase;
    char num;
    char sym;
    bool loop;

    std::string password;
    std::string lAlph = "abcdefghijklmnopqrstuvwxyz";
    std::string uAlph = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string numbers = "0123456789";
    std::string symbols = "!@#$%^&*()_+-=[]{}|;:,.<>/?";
    std::string chars;

    //ask user for length of password
    std::cout << "Enter length of password: ";
    std::cin >> length;

    //loop if no option is selected
    do {
        loop = false;
        std::cout << "Do you want lowercase letters? (y/n)" << std::endl;
        std::cin >> lCase;
        if (lCase == 'y') {
            chars += lAlph;
            range += 26;
        }

        std::cout << "Do you want uppercase letters? (y/n)" << std::endl;
        std::cin >> uCase;
        if (uCase == 'y') {
            chars += uAlph;
            range += 26;
        }

        std::cout << "Do you want numbers? (y/n)" << std::endl;
        std::cin >> num;
        if (num == 'y') {
            chars += numbers;
            range += 10;
        }

        std::cout << "Do you want symbols? (y/n)" << std::endl;
        std::cin >> sym;
        if (sym == 'y')
        {
            chars += symbols;
            range += symbols.length();
        }

        if (range == 0)
        {
            std::cout << "You need to select at least one character type." << std::endl;
            loop = true;
        }
    } while (loop);

    std::uniform_int_distribution<int> uni(0, range - 1);

    for (int i = 0; i < length; i++)
    {
        password += chars[uni(rng)];
    }

    std::cout << "Generated Password: " << password << std::endl;
}

void loginSystem::printTable() {
    queryResults.clear(); // Clear the previous query results

    // Execute the SELECT query
    std::string selectQuery = "SELECT * FROM users;";
    char* errorMsg;
    int rc = sqlite3_exec(db, selectQuery.c_str(), loginSystem::staticCallback, &queryResults, &errorMsg);

    // Check for errors
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        return;
    }

    // Print the query results
    for (const auto& row : queryResults) {
        for (const auto& field : row) {
            std::cout << field << " ";
        }
        std::cout << std::endl;
    }
}
