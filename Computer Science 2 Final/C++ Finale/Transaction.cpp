// Transaction.cpp
#include "Transaction.h"
#include "Patron.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cctype>

Transaction::Transaction(int id, const std::string& transType, double amt)
    : patronId(id), type(transType), amount(amt) {
    timestamp = std::time(nullptr);
}

int Transaction::getPatronId() const { return patronId; }
std::string Transaction::getType() const { return type; }
double Transaction::getAmount() const { return amount; }
std::time_t Transaction::getTimestamp() const { return timestamp; }

std::string Transaction::getFormattedDate() const {
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&timestamp));
    return buffer;
}

void Transaction::display() const {
    std::cout << "Transaction [" << getFormattedDate() << "] - "
              << type << ": $" << std::fixed << std::setprecision(2) << amount
              << " (Patron ID: " << patronId << ")\n";
}

// ====================== PAYMENT SYSTEM ======================
void Transaction::processPayment(Library& lib) {
    int userId = 0;
    std::string userName;
    Patron* found = nullptr;

    std::cout << "\n=== Pay Balance ===\n";
    std::cout << "Enter your ID number (or 0 to search by name): ";
    
    if (!(std::cin >> userId)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (userId == 0) {
        std::cout << "Please enter your name: ";
        std::getline(std::cin, userName);
        if (userName.empty()) {
            std::cout << "Name cannot be empty.\n";
            return;
        }
    }

    // Find patron
    for (auto &p : lib.getPatrons()) {
        if ((userId != 0 && p.getId() == userId) ||
            (userId == 0 && strcasecmp(p.getName().c_str(), userName.c_str()) == 0)) {
            found = &p;
            break;
        }
    }

    if (found == nullptr) {
        std::cout << "No user found.\n";
        return;
    }

    double currentDebt = found->getDebt();
    double currentBalance = found->getBalance();

    std::cout << "\nUser: " << found->getName() << " (ID: " << found->getId() << ")\n";
    std::cout << "Current Debt:    $" << std::fixed << std::setprecision(2) << currentDebt << "\n";
    std::cout << "Account Balance: $" << std::fixed << std::setprecision(2) << currentBalance << "\n\n";

    if (currentDebt <= 0.0) {
        std::cout << " You have no outstanding debt!\n";
        return;
    }

    std::cout << "Use account balance to pay? (Y/N): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    double payment = 0.0;
    std::string transType = "Payment";

    if (std::toupper(choice) == 'Y' && currentBalance > 0.0) {
        payment = std::min(currentBalance, currentDebt);
        found->setBalance(currentBalance - payment);
        found->setDebt(currentDebt - payment);
        transType = "Balance Payment";

        std::cout << " $" << std::fixed << std::setprecision(2) << payment 
                  << " paid from balance.\n";
    } 
    else {
        std::cout << "Enter payment amount: $";
        if (!(std::cin >> payment) || payment <= 0.0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid amount.\n";
            return;
        }

        double actual = std::min(payment, currentDebt);
        found->setDebt(currentDebt - actual);

        if (payment > currentDebt) {
            found->setBalance(found->getBalance() + (payment - currentDebt));
            std::cout << " Debt cleared with overpayment.\n";
        } else {
            std::cout << " Payment accepted.\n";
        }
    }

    // Create Transaction object and display
    Transaction tx(found->getId(), transType, payment);
    tx.display();

    lib.saveJsonData("data.json");
    std::cout << " Payment completed and saved.\n";
}