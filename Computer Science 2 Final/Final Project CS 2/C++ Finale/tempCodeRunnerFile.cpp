#include "Library.h"

#include <limits> // allows cin to discard the rest of current inpuit line. 
#include <iostream>
#include <vector> 
using namespace std;

/***** Global Variables: *********/

bool running = true;
Library lib;

void adminmenuSelection();
void VerifyAdmin();



void menuSelection(int x) {
    if (x == 0) {
        cout << "\nWelcome to the Toon library\nThis is a universal library for all cartoon characters to rent book!\nPlease make a selection:"
                << "1) Register new user\n"
                << "2) Return book\n"
                << "3) Rent book\n"
                << "4) Donate book\n"
                << "5) Display available books\n"
                << "6) Admin\n";

    } else if (x == 1) {
        cout << "\nMain menu:\n"
                << "1) Register new user\n"
                << "2) Return book\n"
                << "3) Rent book\n"
                << "4) Donate book\n"
                << "5) Display available books\n"
                << "6) Admin\n";
    }
    else if(x == -1) { 
        running = false; 
        cout << "Sorry the Library is currently closed. \n";
    }
    else {
        cout << x << " Error: Default menu-Selection Function called\n";
    }
}

bool askRepeat() {
    char ch = '\0';
    while (true) {
        std::cout << "\nWould you like to make another selection? Y/N: ";
        if (!(std::cin >> ch)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Input error. Exiting.\n";
            return false; // treat extraction error as exit
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (ch == 'Y') return true;
        if (ch == 'N') return false;
        std::cout << "Invalid selection. Please enter Y or N.\n";
    }
}

void adminmenuSelection() {
    cout << "\nAdmin Menu:\n"
         << "1) Show all data\n"
         << "2) View all users\n"
         << "3) View books available\n"
         << "4) View rented books\n"
         << "5) Exit Admin Menu\n";

    int adminInput;
    while (true) {
        cout << "Enter your selection: ";
        if (!(cin >> adminInput)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 5.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (adminInput) {
            case 1: lib.displayJsonData(); break;
            case 2: cout << "Viewing all users...\n"; break;
            case 3: cout << "Viewing books available...\n"; break;
            case 4: cout << "Viewing rented books...\n"; break;
            case 5: cout << "Exiting Admin Menu.\n"; return;
            default: cout << "Please choose a valid option (1-5).\n"; break;
        }
    }
}

void VerifyAdmin() {
    string password;
    cout << "Enter admin password: ";
    cin >> password;

    if (password == "chad7") {
        cout << "Access granted. Welcome, Chad\n";
        adminmenuSelection();
    } else {
        cout << "Access denied. Incorrect password.\n";
    }
}

void registerUser() {
    std::string name;
    int id;

    std::cout << "Enter name: ";
    std::getline(std::cin, name);

    std::cout << "Enter ID: ";
    std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Patron p(name, id);
    lib.addPatron(p);

    std::cout << "User registered successfully!\n";
}

int main() {

// 0-Standard greeting 1-Menu only -1 closed.
menuSelection(0); 

while (running) {
    int userInput;

while (!(std::cin >> userInput)) {
    std::cout << "Invalid input. Please enter a correct selection below: ";
    menuSelection(1); 
    std::cin.clear(); 
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ****** Login to use seleced request. 
switch (userInput) {
    case 0: cout << "You entered:Exit \n"; break;
    case 1: registerUser(); break;
    case 2: cout << "You entered:2\n"; running = false; break;
    case 3: cout << "You entered:3\n"; running = false; break;
    case 4: cout << "You entered:4\n"; running = false; break;
    case 5: cout << "You entered:5\n"; running = false; break;
    case 6: cout << "You entered:6\n"; running = false; VerifyAdmin(); break;
    default: cout << "Please choose a valid option\n"; break;
}

 running = askRepeat();
  if (!running) std::cout << "\nThank you for visiting the library.\n"; 
    else menuSelection(0); 
} // end of running loop. 









// End of program.
return 0; 
}