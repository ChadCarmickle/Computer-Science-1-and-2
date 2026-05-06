// Library.h
// Purpose: Manages the entire library system (patrons, books, data, actions).

#pragma once
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include "Patron.h"
#include "Book.h"

void pauseSystem(const std::string &msg);

class Library {
private:
    std::vector<Patron> patrons;
    std::vector<Book*> books;

public:
    Library() = default;

    ~Library(); // Destructor to clean up dynamically allocated books

    void registerUser();
    Patron* verifyUser();
    void verifyAdmin();

    std::vector<Patron>& getPatrons() { return patrons; }
    const std::vector<Patron>& getPatrons() const { return patrons; }

    std::vector<Book*>& getBooks() { return books; }
    const std::vector<Book*>& getBooks() const { return books; }

    Patron& addPatron(const Patron &p);
    Patron& addNewUser(const std::string& name);

    void loadJsonData(const std::string &filename = "data.json");
    void saveJsonData(const std::string &filename = "data.json") const;

    void displayJsonData() const;
    void displayPatronBooks(const Patron& patron) const;

    int nextId() const;

    void addBook(Book* book);
    Book* findBook(const std::string& title) const;
    bool isBookAvailable(const std::string& title) const;
    
    bool canPatronBorrow(const Patron& patron) const;
    void rentBookToPatron(Patron& patron);
    
    bool returnBookForPatron(Patron& patron);
    bool returnBookById(int id, const std::string &filename);
    bool returnBookByName(const std::string &name, const std::string &filename);

    void adminMenu();
    void searchBooks() const;
    
    bool isBookGloballyAvailable(const std::string& title) const;
};