// Book.h
// Connects to Book.cpp
// Purpose: Represents a single book (title, author, genre, status) and related actions.

#pragma once
#include <string>
#include <iostream>
#include <ctime> // For timestamping. 


enum class Genre { 
    Fiction, 
    NonFiction, 
    Mystery, 
    Science, 
    Biography, 
    Fantasy, 
    Other 
};

enum class BookStatus { 
    CheckedIn,     // Available / on shelf
    CheckedOut     // Borrowed by a patron
};

class Book {
protected:
    std::string title;
    std::string author;
    Genre genre;
    BookStatus status = BookStatus::CheckedIn; // Default to available
    std::time_t rentalDate = 0; // Timestamp for when the book was rented (for overdue calculations)

public:
    Book(std::string t, std::string a, Genre g);
    virtual ~Book() = default;

    virtual void displayInfo() const;
    void setStatus(BookStatus s);
    
// Getters and Setters
    BookStatus getStatus() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    Genre getGenre() const;



    // Rental date management
    void setRentalDate();
    std::time_t getRentalDate() const;

    std::string getRentalDateFormatted() const;
    int getDaysSinceRental() const;        // Useful for late fee later
    

    // implement later. 
    bool operator==(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Book& b);


};