// EBook.cpp
// Purpose: Represents a digital book (inherits from Book).

#include "EBook.h"

EBook::EBook(std::string t, std::string a, Genre g, double size)
    : Book(std::move(t), std::move(a), g), fileSizeMB(size) {}

void EBook::displayInfo() const {
    std::cout << "=== E-Book ===\n";
    Book::displayInfo();  // Call base class version
    std::cout << "File Size: " << fileSizeMB << " MB\n";
}