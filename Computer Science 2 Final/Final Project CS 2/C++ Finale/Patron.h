// Patron.h
// Connects to Patron.cpp 
// Represents a single user (name, ID, borrowed books, balance, debt, and personal actions like borrow/return).

#pragma once
#include <string>
#include <vector>
#include "Book.h"

class Patron {
private:
    std::string name;
    int id;
    std::vector<Book*> borrowedBooks;   // Now holds Book pointers
    double balance = 0.0;
    double debt = 0.0;


public:
    Patron();
    Patron(const std::string &n, int i);

    std::string getName() const;
    int getId() const;

    void borrowBook(Book* b);
    bool returnBook(Book* b);           // Returns true if successful
    bool hasBorrowedBooks() const;
    const std::vector<Book*>& getBorrowedBooks() const;

    void setBalance(double v);
    double getBalance() const;

    void setDebt(double v);
    double getDebt() const;

    void makePayment(double amount);
    void addFine(double amount);

    void displayPatron() const;

    bool operator==(const Patron& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Patron& p);
};