#ifndef ENTITIES_H
#define ENTITIES_H

#include <QString>
#include <QDate>
#include <vector>

/**
 * ENTITY: ComplianceDoc
 * Based on Section 2.5 of HintonMarket Overview.
 * Stores details for Business Licenses, Insurance, and Food Handler Certs.
 */
struct ComplianceDoc {
    QString type;        // e.g., "Business License", "Liability Insurance"
    QString docNumber;   // The ID number of the document
    QDate expiryDate;    // Must be checked against market dates
};

/**
 * ENTITY: User (Base Class)
 * Requirement: 10 unique users (4 Food, 4 Artisan, 1 Op, 1 Admin).
 */
class User {
public:
    QString username;
    QString fullName;
    QString role; // "Vendor", "Operator", or "Admin"

    User(QString u, QString f, QString r) : username(u), fullName(f), role(r) {}
    virtual ~User() {} // Virtual destructor for proper OOP polymorphism
};

/**
 * ENTITY: Vendor (Derived Class)
 * Based on Section 2.4 and 2.5.
 */
class Vendor : public User {
public:
    QString businessName;
    QString category; // "Food" or "Artisan"
    QString email;
    QString phone;
    QString address;

    // List of documents the vendor has uploaded
    std::vector<ComplianceDoc> documents;

    Vendor(QString u, QString f, QString cat) 
        : User(u, f, "Vendor"), category(cat) {}

    // Helper logic: Check if vendor has all required docs for their category
    bool isFullyCompliant() {
        if (category == "Food") return documents.size() >= 3; // License, Insurance, FoodCert
        return documents.size() >= 2; // License, Insurance
    }
};

/**
 * ENTITY: MarketDate
 * Based on Section 2.1 and 2.2.
 * Note: D1 limits are 2 Food and 2 Artisan stalls.
 */
class MarketDate {
public:
    QDate date;
    
    // Pointers to the Vendors who successfully booked
    std::vector<Vendor*> bookedFoodVendors;
    std::vector<Vendor*> bookedArtisanVendors;

    // Capacity constants for Deliverable 1
    const int MAX_FOOD = 2;
    const int MAX_ARTISAN = 2;

    MarketDate(QDate d) : date(d) {}

    bool isFoodFull() const { return bookedFoodVendors.size() >= MAX_FOOD; }
    bool isArtisanFull() const { return bookedArtisanVendors.size() >= MAX_ARTISAN; }
};

#endif // ENTITIES_H