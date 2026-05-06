// Book.cpp
// Purpose: Represents a single book (title, author, genre, status) and related actions.

#include "Book.h"
#include "EBook.h"       
#include "PrintedBook.h" 

Book::Book(std::string t, std::string a, Genre g)
    : title(std::move(t)), author(std::move(a)), genre(g), status(BookStatus::CheckedIn) {}


// Admin feature - display book info in a nice format
void Book::displayInfo() const {
    std::cout << "Title: " << title 
              << "\nAuthor: " << author
              << "\nGenre: ";
    
    switch(genre) {
        case Genre::Fiction:    std::cout << "Fiction"; break;
        case Genre::NonFiction: std::cout << "Non-Fiction"; break;
        case Genre::Mystery:    std::cout << "Mystery"; break;
        case Genre::Science:    std::cout << "Science"; break;
        case Genre::Biography:  std::cout << "Biography"; break;
        case Genre::Fantasy:    std::cout << "Fantasy"; break;
        default:                std::cout << "Other"; break;
    }
    
    std::cout << "\nStatus: " << (status == BookStatus::CheckedIn ? "Available" : "Checked Out");
    
    // Show correct type using polymorphism
    if (dynamic_cast<const EBook*>(this)) {
        std::cout << "\nType: E-Book";
    } else if (dynamic_cast<const PrintedBook*>(this)) {
        std::cout << "\nType: Printed Book";
    } else {
        std::cout << "\nType: Book";
    }
    std::cout << "\n";
}

// === Getters and Setters === 
// Check if book is available (CheckedIn) or not (CheckedOut)
BookStatus Book::getStatus() const { return status; }
void Book::setStatus(BookStatus s) { status = s; }


// === Getters for title, author, genre ===
std::string Book::getTitle() const { return title; }
std::string Book::getAuthor() const { return author; }
Genre Book::getGenre() const { return genre; }


// === Rental date management ===
void Book::setRentalDate() {
    rentalDate = std::time(nullptr);
}
std::time_t Book::getRentalDate() const {
    return rentalDate;
}
std::string Book::getRentalDateFormatted() const {
    if (rentalDate == 0) return "Not rented";
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&rentalDate));
    return buffer;
}

int Book::getDaysSinceRental() const {
    if (rentalDate == 0) return -1; // Not rented
    std::time_t now = std::time(nullptr);
    double diffInSeconds = std::difftime(now, rentalDate);
    return static_cast<int>(diffInSeconds / (60 * 60 * 24)); // Convert seconds to days
}


bool Book::operator==(const Book& other) const {
    return title == other.title && author == other.author;
}

std::ostream& operator<<(std::ostream& os, const Book& b) {
    b.displayInfo();
    return os;
}