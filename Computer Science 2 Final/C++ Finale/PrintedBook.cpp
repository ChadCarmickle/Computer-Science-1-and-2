#include "PrintedBook.h"

PrintedBook::PrintedBook(std::string t, std::string a, Genre g, int pages)
    : Book(std::move(t), std::move(a), g), pageCount(pages) {}

void PrintedBook::displayInfo() const {
    std::cout << "=== Printed Book ===\n";
    Book::displayInfo();  // Call base class version
    std::cout << "Pages: " << pageCount << "\n";
}