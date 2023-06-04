#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <gtk/gtk.h>

// SQLite database connection
sqlite3* db;

// Function to initialize the SQLite database
void initializeDatabase() {
    int rc;

    rc = sqlite3_open("data.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Create necessary tables if they don't exist
    char* createDepositsTable = "CREATE TABLE IF NOT EXISTS deposits (id INTEGER PRIMARY KEY, name TEXT, date TEXT, amount REAL, notes TEXT);";
    rc = sqlite3_exec(db, createDepositsTable, 0, 0, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Add additional create table statements for other data types if needed
}

// Function to close the SQLite database connection
void closeDatabase() {
    sqlite3_close(db);
}

// Function to handle the Deposit form submission
void handleDepositForm() {
    char name[100];
    char date[100];
    double amount;
    char notes[1000];

    printf("Enter Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Date: ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0';

    printf("Enter Amount: ");
    scanf("%lf", &amount);
    getchar();

    printf("Enter Notes: ");
    fgets(notes, sizeof(notes), stdin);
    notes[strcspn(notes, "\n")] = '\0';

    // Insert the deposit data into the database using SQLite queries
    char query[3000];
    // In handleDepositForm function
    snprintf(query, sizeof(query), "INSERT INTO deposits (name, date, amount, notes) VALUES ('%s', '%s', %.2lf, '%s');", name, date, amount, notes);

    int rc = sqlite3_exec(db, query, 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Deposit saved successfully\n");
}

// Function to handle the Loans form submission
void handleLoansForm() {
    char name[100];
    char date[100];
    float amount;
    char notes[1000];

    printf("Loans Form\n");
    printf("Enter name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter date: ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0';

    printf("Enter amount: ");
    scanf("%f", &amount);
    getchar(); // Clear the newline character from the input buffer

    printf("Enter notes: ");
    fgets(notes, sizeof(notes), stdin);
    notes[strcspn(notes, "\n")] = '\0';

    // Validate form data (add your validation logic here)
    if (strlen(name) == 0 || strlen(date) == 0 || amount <= 0) {
        printf("Invalid form data. Please try again.\n");
        return;
    }

    // Code to insert the Loans data into the SQLite database
    char insertQuery[3000];
    snprintf(insertQuery, sizeof(insertQuery), "INSERT INTO loans (name, date, amount, notes) VALUES ('%s', '%s', %.2f, '%s');", name, date, amount, notes);

    int rc = sqlite3_exec(db, insertQuery, 0, 0, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    printf("Loans data inserted successfully.\n");
}

// Function to handle the New Client form submission
void handleNewClientForm() {
    // Retrieve form data
    char name[100];
    printf("Enter name: ");
    scanf("%s", name);

    char address[100];
    printf("Enter address: ");
    scanf("%s", address);

    char contact[100];
    printf("Enter contact: ");
    scanf("%s", contact);

    // Validate form data (add your validation logic here)
    if (strlen(name) == 0 || strlen(address) == 0 || strlen(contact) == 0) {
        printf("Invalid form data. Please try again.\n");
        return;
    }

    // Insert data into the database
    char insertQuery[3000];
    sprintf(insertQuery, "INSERT INTO clients (name, address, contact) VALUES ('%s', '%s', '%s');",
            name, address, contact);

    int rc = sqlite3_exec(db, insertQuery, 0, 0, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    printf("New client data inserted successfully.\n");
}

// Function to display the Dashboard
void displayDashboard() {
    // Retrieve data from the database
    char query[3000];
    sprintf(query, "SELECT COUNT(*) FROM deposits;");
    int depositCount = 0;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        depositCount = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    // Display statistics
    printf("Deposit Count: %d\n", depositCount);
    // Add additional statistics or calculations based on your requirements
}

// Function to display the Client List
void displayClientList() {
    // Retrieve data from the database
    char query[1000];
    sprintf(query, "SELECT * FROM clients;");
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Display the client list
    printf("Client List:\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int clientId = sqlite3_column_int(stmt, 0);
        const unsigned char* clientName = sqlite3_column_text(stmt, 1);

        printf("Client ID: %d\n", clientId);
        printf("Client Name: %s\n", clientName);
        printf("--------------------\n");
    }

    sqlite3_finalize(stmt);
}

// Function to handle the user login
int handleLogin() {
    char username[100];
    char password[100];

    printf("Login\n");
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    
    int loggedIn = 0;
     // Validate the username and password
    if (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0) {
        loggedIn = 1;
        printf("Login successful!\n");
    } else {
        printf("Invalid username or password. Please try again.\n");
    }

    return loggedIn;
}

int main() {
    // Initialize the SQLite database
    initializeDatabase();

    // Perform user login
    int loggedIn = handleLogin();
    if (!loggedIn) {
        printf("Login failed. Exiting...\n");
        closeDatabase();
        return 0;
    }

    // Code for UI enhancement libraries (recommendations)
    // You can choose and install the libraries of your choice for UI enhancement
    // Some popular options are: ncurses, GTK+, Qt, SDL, or wxWidgets

    // Main menu and user interface code

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "My Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Display the window
    gtk_widget_show_all(window);

    // Run the main GTK loop
    gtk_main();

    return 0;
}

    
    // You can modify the code based on the chosen UI library

    int option;
    do {
        printf("\nMain Menu\n");
        printf("1. Deposits\n");
        printf("2. Loans\n");
        printf("3. New Client\n");
        printf("4. Dashboard\n");
        printf("5. Client List\n");
        printf("0. Exit\n");
        printf("Enter option: ");
        scanf("%d", &option);
        getchar(); // Clear the newline character from the input buffer

        switch (option) {
            case 1:
                handleDepositForm();
                break;
            case 2:
                handleLoansForm();
                break;
            case 3:
                handleNewClientForm();
                break;
            case 4:
                displayDashboard();
                break;
            case 5:
                displayClientList();
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid option\n");
                break;
        }
    } while (option != 0);

    // Close the SQLite database connection before exiting the program
    closeDatabase();

    return 0;
}
