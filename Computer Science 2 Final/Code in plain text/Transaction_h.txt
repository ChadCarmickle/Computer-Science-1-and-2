// Transaction.h
// Connects to Transaction.cpp
// Purpose: Records a financial or borrowing transaction (payment, fine, etc.).
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <vector>
#include "Library.h"

class Transaction {
private:
    int patronId;
    std::string type;        // "Payment", "Balance Payment", "Fine", etc.
    double amount;
    std::time_t timestamp;

public:
    // Constructor
    Transaction(int id, const std::string& transType, double amt);

    // Getters
    int getPatronId() const;
    std::string getType() const;
    double getAmount() const;
    std::time_t getTimestamp() const;
    std::string getFormattedDate() const;

    void display() const;

    // === Static Payment System (Main Entry Point) ===
    static void processPayment(Library& lib);

    // Optional: Get all transactions (can be extended later)
    static std::vector<Transaction> getAllTransactions(); // Future feature
};

#endif