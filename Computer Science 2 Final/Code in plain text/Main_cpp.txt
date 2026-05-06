// main.cpp
#include "Library.h"
#include "Patron.h"
#include "Transaction.h"
#include "Book.h"
#include "EBook.h"
#include "PrintedBook.h"

#include <limits>
#include <iostream>
#include <vector> 
#include <cctype>
#include <string>
#include <cstring>
#include <ctime>

using namespace std;

/***** Global Variables: *********/
bool running = true;
Library lib;
/*********************************/

void menuSelection(int x) {
    if (x == 0) {
        cout << "\nWelcome to the Toon library\n"
                << "This is a universal library for all cartoon characters to rent book!\n"
                << "Please make a selection:\n"
                << "1) Register new user\n"
                << "2) Return book\n"
                << "3) Rent book\n"
                << "4) Pay balance\n"
                << "5) Admin\n"
                << "6) Exit\n";
    } else if (x == 1) {
        cout << "\nMain menu:\n"
                << "1) Register new user\n"
                << "2) Return book\n"
                << "3) Rent book\n"
                << "4) Pay balance\n"
                << "5) Admin\n"
                << "6) Exit\n";
    }
}

bool askRepeat() {
    char ch;
    while (true) {
        cout << "\nWould you like to return to the main menu? Y/N: ";
        cin >> ch;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return false;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        ch = toupper(ch);
        if (ch == 'Y') return true;
        if (ch == 'N') return false;
        cout << "Invalid selection.\n";
    }
}

int main() {
    // Load data
    try {
        lib.loadJsonData("data.json");
    } catch (const exception& e) {
        cerr << "Failed to load data: " << e.what() << endl;
        return 1;
    }

    srand(time(nullptr));
    menuSelection(0);

    while (running) {
        int userInput;
        while (!(cin >> userInput)) {
            cout << "Invalid input. Please enter a number.\n";
            menuSelection(1);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (userInput) {
            case 1: lib.registerUser(); break;
            case 2: {
                Patron* found = lib.verifyUser();
                if (found && lib.returnBookForPatron(*found)) {
                    lib.saveJsonData("data.json");
                }
                break;
            }
            case 3: {
                Patron* found = lib.verifyUser();
                if (found) lib.rentBookToPatron(*found);
                break;
            }
            case 4: Transaction::processPayment(lib); break;
            case 5: lib.verifyAdmin(); break;
            case 6: running = false; break;
            default: cout << "Please choose a valid option (1-6).\n"; break;
        }

        if (!running) {
            // === Random Goodbye Message ===
            int msg = rand() % 24;
        switch(msg) {
            case 0:  cout << "\nThank you for visiting the Toon Library!\n"; break;
            case 1:  cout << "\nIt’s been a magical journey! Reading a book is just like visiting the Hundred Acre Wood!\n"; break;
            case 2:  cout << "\nGoodbye! May your stories be as lively as Bugs Bunny's antics!\n"; break;
            case 3:  cout << "\nSo long! A great book will have you dream like Alice in Wonderland!\n"; break;
            case 4:  cout << "\nThanks for visiting! If you see Scooby Doo, tell him there are no monsters here, only great stories!\n"; break;
            case 5:  cout << "\nFarewell! Don't forget, every good book is a portal to another world!\n"; break;
            case 6:  cout << "\nSo long! May your next read take you to the stars, like Star Wars!\n"; break;
            case 7:  cout << "\nSee you later! If you see Jake the Dog or Finn the Human, tell them a good book is just as good as an adventure!\n"; break;
            case 8:  cout << "\nSo long! Just like Kim Possible, you can do anything... even check out more books!\n"; break;
            case 9:  cout << "\nFarewell, friend! May your imagination take you far, like Hiro and Baymax!\n"; break;
            case 10: cout << "\nAdios! From one curious cat to another, keep exploring!\n"; break;
            case 11: cout << "\nFarewell! Be like Tigger and bounce back soon!\n"; break;
            case 12: cout << "\nSo long, friend! Like Arthur always said, be ready for another adventure!\n"; break;
            case 13: cout << "\nGoodbye! May your next book be as wild as the Wild Thornberrys!\n"; break;
            case 14: cout << "\nThanks for stopping by! Just like Naruto, a book can take you on an incredible journey!\n"; break;
            case 15: cout << "\nSo long! Unfortunately none of my books reveal the location of the 'One Piece'. You have to find that yourself!\n"; break;
            case 16: cout << "\nHave a great day! Try your best to avoid any Shinigamis! (Japanese Spirits who invite humans to the underworld)\n"; break;
            case 17: cout << "\nGoodbye! Embrace every moment to its fullest!\n"; break;
            case 18: cout << "\nGoodbye! Remember the first rule in alchemy: 'Humankind cannot gain anything without first giving something in return'!\n"; break;
            case 19: cout << "\nCatch you later! Remember to be careful if you time travel with microwaves.\n"; break;
            case 20: cout << "\nThanks for dropping by! Check out the Pokemon Center next time!\n"; break;
            case 21: cout << "\nHave a great day! Remember to jump into new adventures like Mario!\n"; break;
            case 22: cout << "\nSee you next time, whether you're crafting a pickaxe or enchanting a sword!\n"; break;
            case 23: cout << "\nGoodbye! Like Aladdin, may your next book take you to a magical realm!\n"; break;
            default: cout << "\nThank you for visiting the Toon Library!\n"; break;
        }
    }
        else {
            running = askRepeat();
            if (running) menuSelection(0);
        }
    }

    cout << "\nThank you for visiting the library.\n";
    return 0;
}