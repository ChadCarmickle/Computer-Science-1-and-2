// Library.cpp
// Purpose: Manages the entire library system (list of patrons, list of books, loading/saving data, coordinating actions).
#include "Library.h"
#include "Patron.h"
#include "Transaction.h"
#include "Book.h"
#include "EBook.h"
#include "PrintedBook.h"

#include <fstream>                     // File Input / Output
#include <iostream>                   // Console Input / Output
#include <sstream>                   // String Stream for parsing
#include <cstdlib>                  // For system("pause") on Windows
#include <vector>                  // For using std::vector
#include <cctype>                 // For character handling (e.g., std::tolower)
#include <string>                // For using std::string
#include <cstring>              // For C-style string functions (e.g., strcasecmp)
#include <algorithm>           // For std::find_if and std::transform
#include <limits>             // For std::numeric_limits
#include <ctime>             // For time-related functions (e.g., std::time, std::localtime)
#include <filesystem>       // For file handling and backup
#include <iomanip>         // For std::setprecision in Transaction display

#include "nlohmann/json.hpp"      // JSON library header    
using json = nlohmann::json;     // Alias for convenience
namespace fs = std::filesystem; // Alias for filesystem namespace

void pauseSystem(const std::string &msg) {
    std::cout << "System paused. Press Enter to continue...\n";
    std::cout << msg << "\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/********************** Core Function *******************************/
// === JSON Data Handling ===
void Library::loadJsonData(const std::string &filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cout << "No previous data found. Starting fresh.\n";
        return;
    }

    json j;
    try {
        in >> j;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to parse JSON: " << e.what() << "\n";
        return;
    }
    patrons.clear();     
    books.clear();      

    for (const auto &item : j) {
        std::string name = item.value("name", "");
        int id = item.value("id", 0);

        if (name.empty() || id == 0) continue;

        Patron p(name, id);
        p.setBalance(item.value("balance", 0.0));
        p.setDebt(item.value("debt", 0.0));

        // New format (preferred)
        if (item.contains("borrowedBooks")) {
            for (const auto &b : item["borrowedBooks"]) {
                if (b.is_null()) continue;

                std::string title = b.value("title", "");
                if (title.empty()) continue;
                std::string author = b.value("author", "Unknown");

                // *** IMPORTANT: Always recreate/find book globally ***
                Book* book = findBook(title);
                if (!book) {
                    auto* newBook = new PrintedBook(title, author, Genre::Other, 300);
                    addBook(newBook);
                    book = newBook;
                }
                p.borrowBook(book);
            }
        } 

        patrons.push_back(p);
    }

    std::cout << "Loaded " << patrons.size() << " patrons and " 
              << books.size() << " books from " << filename << ".\n";
}

void Library::saveJsonData(const std::string &filename) const {
    std::string backup = filename + ".bak";

    // Remove old backup if it exists to avoid conflicts
    if (fs::exists(backup)) {
        try {
            fs::remove(backup);
        } catch (...) {
            // Ignore if we can't delete
        }
    }

    // Create new backup
    if (fs::exists(filename)) {
        try {
            fs::copy(filename, backup, fs::copy_options::overwrite_existing);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not create backup: " << e.what() << "\n";
        }
    }

    // Write new data
    json j = json::array();

    for (const auto &p : patrons) {
        json patronJson = {
            {"id", p.getId()},
            {"name", p.getName()},
            {"balance", p.getBalance()},
            {"debt", p.getDebt()}
        };

        json borrowed = json::array();
        const auto& booksList = p.getBorrowedBooks();

        for (size_t i = 0; i < 3; ++i) {
            if (i < booksList.size() && booksList[i]) {
                const Book* b = booksList[i];
                json bookEntry = {
                    {"title", b->getTitle()},
                    {"author", b->getAuthor()},
                    {"genre", static_cast<int>(b->getGenre())},
                    {"status", "CheckedOut"},
                    {"rentalDate", b->getRentalDate()}
                };
                borrowed.push_back(bookEntry);
            } else {
                borrowed.push_back(nullptr);
            }
        }

        patronJson["borrowedBooks"] = borrowed;
        j.push_back(patronJson);
    }

    std::ofstream out(filename);
    if (out) {
        out << j.dump(4) << '\n';
        std::cout << " Data saved successfully to " << filename << "\n";
    } else {
        std::cerr << " Failed to save data to " << filename << "\n";
    }
}


// === Display Data (for debugging) ===
void Library::displayJsonData() const {
    for (const auto &p : patrons) {
        p.displayPatron();
        
        std::cout << "Borrowed Books:\n";
        const auto& borrowed = p.getBorrowedBooks();
        
        if (borrowed.empty()) {
            std::cout << "   No books borrowed.\n";
        } else {
            for (const auto* b : borrowed) {
                if (b) {
                    b->displayInfo();           // Shows E-Book or Printed Book correctly
                    std::cout << "-------------------\n";
                }
            }
        }
        std::cout << "====================================\n";
    }
}




// === User Registration ===
void Library::registerUser() {
    std::string name;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Enter your full name: ";
        std::getline(std::cin, name);

        if (!name.empty()) break;

        std::cout << "Error: Name cannot be empty. Please try again.\n";
    }

    Patron& newPatron = addNewUser(name);

    std::cout << name << " registered successfully! (ID: " 
              << newPatron.getId() << ")\n";

    char choice;
    while (true) {
        std::cout << "Would you like to check out a book now? (Y/N): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = std::toupper(choice);

        if (choice == 'Y' || choice == 'N') break;
        std::cout << "Invalid input. Please enter Y or N.\n";
    }

    if (choice == 'Y') {
        rentBookToPatron(newPatron);
    }

    saveJsonData("data.json");
    std::cout << "Data saved successfully.\n";
}
// === Add New User (Helper for Registration) ===
Patron& Library::addNewUser(const std::string& name) {
    int id = nextId();
    
    Patron p(name, id);
    p.setBalance(0.0);
    p.setDebt(0.0);

    patrons.push_back(p);
    return patrons.back();   
}

// === ID Generation for New Patrons(users) ===
int Library::nextId() const {
    if (patrons.empty()) {
        return 1000701;
    }

    int maxId = 1000700;
    for (const auto &p : patrons) {
        if (p.getId() > maxId) {
            maxId = p.getId();
        }
    }
    return maxId + 1;
}

// Not implemented, but can later to modify data of newly registered user if needed.
Patron& Library::addPatron(const Patron &p) {
    patrons.push_back(p);
    return patrons.back();   
}

// === Book Management ===
Book* Library::findBook(const std::string& title) const {
    for (Book* b : books) {
        if (b && b->getTitle() == title) {
            return b;
        }
    }
    return nullptr;
}

void Library::addBook(Book* book) {
    if (book) {
        books.push_back(book);
    }
}

// check if book is available (exists and is checked in)
bool Library::isBookAvailable(const std::string& title) const {
    Book* book = findBook(title);
    return book && book->getStatus() == BookStatus::CheckedIn;
}

// Check if patron can borrow (max 3 books and no debt)
bool Library::canPatronBorrow(const Patron& patron) const {
    // For simplicity, let's say a patron can borrow if they have no debt and less than 3 books borrowed
    return patron.getDebt() == 0.0 && patron.getBorrowedBooks().size() < 3;
}

// Return book functions. One by patron, one by id, one by name.
bool Library::returnBookById(int id, const std::string &filename) {
    for (auto &p : patrons) {
        if (p.getId() == id) {
            if (!p.hasBorrowedBooks()) return false;

            // Return first borrowed book for simplicity
            if (!p.getBorrowedBooks().empty()) {
                Book* b = p.getBorrowedBooks()[0];
                p.returnBook(b);
            }
            return true;
        }
    }
    return false;
}

void Library::rentBookToPatron(Patron& patron) {
    if (!canPatronBorrow(patron)) {
        std::cout << " You cannot borrow more books (max 3 or unpaid debt).\n";
        return;
    }

    std::string title;
    std::cout << "\nEnter book title: ";
    std::getline(std::cin, title);

    if (title.empty()) {
        std::cout << "Title cannot be empty.\n";
        return;
    }

    Book* book = findBook(title);

    if (book) {
        if (book->getStatus() != BookStatus::CheckedIn) {
            std::string borrowerName = "Unknown Patron";
            for (const auto& p : patrons) {
                for (const auto* b : p.getBorrowedBooks()) {
                    if (b && b->getTitle() == title) {
                        borrowerName = p.getName();
                        break;
                    }
                }
                if (borrowerName != "Unknown Patron") break;
            }
            std::cout << "Book '" << title << "' is already checked out by " << borrowerName << ".\n";
            return;
        }
    } 
    else {
        // === New Book Creation with Type Selection ===
        std::string author;
        std::cout << "New book. Enter author name: ";
        std::getline(std::cin, author);
        if (author.empty()) author = "Unknown";

        std::cout << "Is this a Printed Book or E-Book?\n";
        std::cout << "1) Printed Book\n";
        std::cout << "2) E-Book\n";

        int bookType = 0;
        while (true) {
            std::cout << "Enter choice (1 or 2): ";
            if (std::cin >> bookType && (bookType == 1 || bookType == 2)) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice. Please enter 1 or 2.\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (bookType == 2) {
            // E-Book
            double fileSize = 0.0;
            while (true) {
                std::cout << "Enter file size in MB: ";
                if (std::cin >> fileSize && fileSize > 0.0) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid size. Please enter a positive number.\n";
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            book = new EBook(title, author, Genre::Other, fileSize);
            std::cout << "New E-Book added to library.\n";
        } else {
            // Printed Book
            int pages = 0;
            while (true) {
                std::cout << "Enter number of pages: ";
                if (std::cin >> pages && pages > 0) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid number. Please enter a positive number.\n";
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            book = new PrintedBook(title, author, Genre::Other, pages);
            std::cout << "New Printed Book added to library.\n";
        }

        addBook(book);
    }

    patron.borrowBook(book);
    book->setRentalDate();

    std::cout << "Successfully rented '" << title << "' to " 
              << patron.getName() << " (ID: " << patron.getId() << ")\n";

    saveJsonData("data.json");
}


bool Library::returnBookByName(const std::string &name, const std::string &filename) {

    for (auto &p : patrons) {
        if (p.getName() == name) {
            if (!p.hasBorrowedBooks()) return false;

            if (!p.getBorrowedBooks().empty()) {
                Book* b = p.getBorrowedBooks()[0];
                p.returnBook(b);
            }
            return true;
        }
    }
    return false;
}

Patron* Library::verifyUser() {
    int userId = 0;
    Patron* found = nullptr;

    while (true) {
        std::cout << "Please enter your ID number (or 0 to search by name): ";
        if (std::cin >> userId) break;

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (userId == 0) {
        std::string name;
        while (true) {
            std::cout << "Please enter your name: ";
            std::getline(std::cin, name);

            if (!name.empty()) break;
            std::cout << "Name cannot be empty. Please try again.\n";
        }

        for (auto &p : patrons) {
            if (strcasecmp(p.getName().c_str(), name.c_str()) == 0) {
                found = &p;
                break;
            }
        }
    } else {
        for (auto &p : patrons) {
            if (p.getId() == userId) {
                found = &p;
                break;
            }
        }
    }

    if (found) {
        std::cout << "Welcome back, " << found->getName() 
                  << " (ID: " << found->getId() << ")\n";
        displayPatronBooks(*found);
    } else {
        std::cout << "No user found.\n";

        char choice;
        while (true) {
            std::cout << "Would you like to register as a new user? (Y/N): ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = std::toupper(choice);

            if (choice == 'Y' || choice == 'N') break;
            std::cout << "Invalid input. Please enter Y or N.\n";
        }

        if (choice == 'Y') {
            registerUser();
        }
    }
    return found;
}
// === Verify Admin ===
void Library::verifyAdmin() {
    std::string password;
    std::cout << "Enter admin password: ";
    std::cin >> password;

    if (password == "chad7") {
        std::cout << "Access granted. Welcome, Chad\n";
        adminMenu();          // ← This line must be adminMenu()
    } else {
        std::cout << "Access denied. Incorrect password.\n";
    }
}

// === Display Patron Books with Due Dates ===
void Library::displayPatronBooks(const Patron& patron) const {
    const auto& books = patron.getBorrowedBooks();
    if (books.empty()) {
        std::cout << "You currently have no borrowed books.\n";
        return;
    }

std::cout << "\nYour Borrowed Books:\n";
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i]) {
            const Book* b = books[i];
            std::time_t dueTime = b->getRentalDate() + (14 * 24 * 60 * 60); // 14 days
            char dueBuffer[80];
            std::strftime(dueBuffer, sizeof(dueBuffer), "%Y-%m-%d", std::localtime(&dueTime));

            int daysRented = b->getDaysSinceRental();
            bool isOverdue = daysRented > 14;

            std::cout << (i+1) << ") " << b->getTitle() 
                      << " by " << b->getAuthor()
                      << "\n   Rented: " << b->getRentalDateFormatted()
                      << " | Due: " << dueBuffer;

            if (isOverdue) {
                int overdueDays = daysRented - 14;
                double fine = overdueDays * 0.50;
                std::cout << "   *** OVERDUE by " << overdueDays 
                        << " days! Estimated fine: $" 
                        << std::fixed << std::setprecision(2) << fine << " ***";
            }
            std::cout << "\n";
        }
    }
}


bool Library::isBookGloballyAvailable(const std::string& title) const {
    Book* book = findBook(title);
    return book && book->getStatus() == BookStatus::CheckedIn;
}    

void Library::searchBooks() const {
    std::cout << "\n=== Search Books ===\n";
    std::cout << "1) By Title\n";
    std::cout << "2) By Author\n";
    std::cout << "3) By Genre\n";

    int choice = 0;
    while (true) {
        std::cout << "Enter choice (1-3): ";
        if (std::cin >> choice && choice >= 1 && choice <= 3) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Please enter 1, 2, or 3.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string query;
    std::cout << "Enter search term: ";
    std::getline(std::cin, query);

    if (query.empty()) {
        std::cout << "Search term cannot be empty.\n";
        return;
    }

    bool foundAny = false;
    std::cout << "\n=== Search Results ===\n";

    for (const Book* b : books) {
        if (!b) continue;

        bool matches = false;

        if (choice == 1 && b->getTitle().find(query) != std::string::npos) matches = true;
        else if (choice == 2 && b->getAuthor().find(query) != std::string::npos) matches = true;
        else if (choice == 3) {
            std::string genreStr;
            switch(b->getGenre()) {
                case Genre::Fiction:    genreStr = "Fiction"; break;
                case Genre::NonFiction: genreStr = "Non-Fiction"; break;
                case Genre::Mystery:    genreStr = "Mystery"; break;
                case Genre::Science:    genreStr = "Science"; break;
                case Genre::Biography:  genreStr = "Biography"; break;
                case Genre::Fantasy:    genreStr = "Fantasy"; break;
                default:                genreStr = "Other"; break;
            }
            if (genreStr.find(query) != std::string::npos) matches = true;
        }

        if (matches) {
            b->displayInfo();
            std::cout << "Status: " << (b->getStatus() == BookStatus::CheckedIn ? "Available" : "Checked Out") << "\n";
            std::cout << "-------------------\n";
            foundAny = true;
        }
    }

    if (!foundAny) {
        std::cout << "No books found matching '" << query << "'.\n";
    }
}

void Library::adminMenu() {
    while (true) {
        std::cout << "\n=== Admin Menu ===\n"
                  << "1) Show all data\n"
                  << "2) View all users\n"
                  << "3) View books available\n"
                  << "4) Display all rented books\n"
                  << "5) Exit Admin Menu\n"
                  << "Enter your selection: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: displayJsonData(); break;
            case 2:
                std::cout << "\n=== All Users ===\n";
                for (const auto& p : patrons) p.displayPatron();
                break;
            case 3:
                std::cout << "\n=== Available Books ===\n";
                for (Book* b : books) {
                    if (b && b->getStatus() == BookStatus::CheckedIn) {
                        b->displayInfo();
                        std::cout << "-------------------\n";
                    }
                }
                break;
            case 4:
                searchBooks();
                break;
            case 5:
                std::cout << "Exiting Admin Menu.\n";
                return;
            default:
                std::cout << "Invalid option.\n";
        }
    }
}

bool Library::returnBookForPatron(Patron& patron) {
    if (!patron.hasBorrowedBooks()) {
        std::cout << "❌ " << patron.getName() 
                  << " has no books currently borrowed.\n";
        return false;
    }

    // Get the first borrowed book (you can enhance this later to choose which book)
    Book* book = nullptr;
    for (Book* b : patron.getBorrowedBooks()) {
        if (b != nullptr) {
            book = b;
            break;
        }
    }

    if (!book) {
        std::cout << " Could not find a book to return.\n";
        return false;
    }

    // Update everything
    book->setStatus(BookStatus::CheckedIn);
    patron.returnBook(book);

    std::cout << book->getTitle() 
              << "' successfully returned by " 
              << patron.getName() << ".\n";

    saveJsonData("data.json");
    return true;
}


// Destructor to clean up dynamically allocated books
Library::~Library() {
    for (auto* b : books) delete b;
}
