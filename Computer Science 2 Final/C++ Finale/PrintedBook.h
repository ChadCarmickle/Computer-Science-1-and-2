// PrintedBook.h
// Connects to PrintedBook.cpp
// Purpose: Represents a physical book (inherits from Book).

#pragma once
#include "Book.h"

class PrintedBook : public Book {
    int pageCount;
public:
    PrintedBook(std::string t, std::string a, Genre g, int pages);
    void displayInfo() const override;
};