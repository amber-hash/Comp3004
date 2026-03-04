#ifndef MARKETMANAGER_H
#define MARKETMANAGER_H

#include "Entities.h"
#include <vector>
#include <queue>
#include <map>

class MarketManager {
private:
    std::vector<User*> allUsers;
    std::vector<MarketDate*> schedule;

    // Waitlists: Map MarketDate to a queue of Vendors
    // We need two: one for Food, one for Artisan
    std::map<QDate, std::queue<Vendor*>> foodWaitlists;
    std::map<QDate, std::queue<Vendor*>> artisanWaitlists;

public:
    MarketManager();
    ~MarketManager();

    // Core Functional Requirements
    void initializeSystem(); // Loads the 10 users and 4 weeks
    User* authenticate(QString username); // Identification logic
    
    // Booking Logic
    bool attemptBooking(Vendor* v, MarketDate* d);
    void addToWaitlist(Vendor* v, MarketDate* d);
    void cancelBooking(Vendor* v, MarketDate* d);

    // Getters for UI
    std::vector<MarketDate*> getSchedule() { return schedule; }
    int getWaitlistPosition(Vendor* v, MarketDate* d);
};

#endif