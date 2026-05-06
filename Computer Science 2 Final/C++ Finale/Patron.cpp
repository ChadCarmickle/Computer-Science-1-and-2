// Patron.cpp
// Purpose: Represents a single user (name, ID, borrowed books, balance, debt, and personal actions like borrow/return).
#include "Patron.h"
#include "Transaction.h"
#include <iostream>

Patron::Patron() : id(0) {}

Patron::Patron(const std::string &n, int i) 
    : name(n), id(i) {}

std::string Patron::getName() const { return name; }
int Patron::getId() const { return id; }

void Patron::borrowBook(Book* b) {
    if (b->getStatus() == BookStatus::CheckedIn) {
        borrowedBooks.push_back(b);
        b->setStatus(BookStatus::CheckedOut);
    }
}

bool Patron::returnBook(Book* b) {
    for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
        if (*it == b) {
            b->setStatus(BookStatus::CheckedIn);
            borrowedBooks.erase(it);
            return true;
        }
    }
    return false;
}

bool Patron::hasBorrowedBooks() const {
    return !borrowedBooks.empty();
}

const std::vector<Book*>& Patron::getBorrowedBooks() const {
    return borrowedBooks;
}

void Patron::setBalance(double v) { balance = v; }
double Patron::getBalance() const { return balance; }

void Patron::setDebt(double v) { debt = v; }
double Patron::getDebt() const { return debt; }

void Patron::makePayment(double amount) {
    if (amount <= 0.0) return;
    if (amount > debt) amount = debt;
    debt -= amount;

    Transaction tx(id, "Payment", amount);
    tx.display();
}

void Patron::addFine(double amount) {
    if (amount > 0) debt += amount;
}

void Patron::displayPatron() const {
    std::cout << "Name: " << name << " | ID: " << id 
              << " | Balance: $" << balance 
              << " | Debt: $" << debt 
              << " | Books Borrowed: " << borrowedBooks.size() << "\n";
}

bool Patron::operator==(const Patron& other) const {
    return id == other.id;
}

std::ostream& operator<<(std::ostream& os, const Patron& p) {
    p.displayPatron();
    return os;
}