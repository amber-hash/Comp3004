#include "MarketManager.h"
#include <algorithm> // for std::remove

// ... (Other functions like initializeSystem go here) ...

bool MarketManager::attemptBooking(Vendor* v, MarketDate* d) {
    if (v->category == "Food") {
        if (!d->isFoodFull()) {
            d->bookedFoodVendors.push_back(v);
            return true; 
        }
    } else {
        if (!d->isArtisanFull()) {
            d->bookedArtisanVendors.push_back(v);
            return true; 
        }
    }
    return false; // Tells the UI to offer the Waitlist instead
}

void MarketManager::cancelBooking(Vendor* v, MarketDate* d) {
    if (v->category == "Food") {
        // 1. Remove vendor from the booked list
        auto& list = d->bookedFoodVendors;
        list.erase(std::remove(list.begin(), list.end(), v), list.end());

        // 2. Automate Waitlist (Functional Requirement 6)
        // If someone is waiting for this specific date...
        if (!foodWaitlists[d->date].empty()) {
            Vendor* nextInLine = foodWaitlists[d->date].front();
            foodWaitlists[d->date].pop();
            
            // Move them to the booked list
            d->bookedFoodVendors.push_back(nextInLine);
            
            // 3. Notify (You'll need a simple way to store this for the Dashboard)
            // nextInLine->addNotification("A stall opened! You have been booked for " + d->date.toString());
        }
    } else {
        // Repeat same logic for Artisan category
        auto& list = d->bookedArtisanVendors;
        list.erase(std::remove(list.begin(), list.end(), v), list.end());

        if (!artisanWaitlists[d->date].empty()) {
            Vendor* nextInLine = artisanWaitlists[d->date].front();
            artisanWaitlists[d->date].pop();
            d->bookedArtisanVendors.push_back(nextInLine);
        }
    }
}