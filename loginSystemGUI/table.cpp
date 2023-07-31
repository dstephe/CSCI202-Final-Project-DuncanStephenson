#include "loginSystem.h"

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
        // Display the error message as a popup window
        QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
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
        // Display the error message as a popup window
        QMessageBox::critical(nullptr, "SQL Error", "SQL error: " + QString(errorMsg));
        sqlite3_free(errorMsg);
    }
}

// Function to print out the user data table
void loginSystem::printTable() {
    queryResults.clear(); // Clear the previous query results
    // Create a QDialog window for displaying the user data table
    QDialog tableDialog;
    tableDialog.setWindowTitle("User Data Table");

    // Create a QLabel widget to display the table data
    QLabel* tableLabel = new QLabel(&tableDialog);
    // Set the label text with the table data
    std::string tableData = "User Data:\n";
    std::string selectQuery = "SELECT * FROM users;";
    char* errorMsg;
    int rc = sqlite3_exec(db, selectQuery.c_str(), loginSystem::staticCallback, &queryResults, &errorMsg);
    for (const auto& row : queryResults) {
        for (const auto& column : row) {
            tableData += column + "    ";
        }
        tableData += "\n";
    }
    tableLabel->setText(QString::fromStdString(tableData));

    // Create a QVBoxLayout to set the layout for the table dialog
    QVBoxLayout* layout = new QVBoxLayout(&tableDialog);
    layout->addWidget(tableLabel);
    tableDialog.setLayout(layout);

    // Show the dialog window and wait for it to be closed
    tableDialog.exec();
}