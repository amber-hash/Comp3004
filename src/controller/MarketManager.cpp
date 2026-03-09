#include "MarketManager.h"
#include "../data/DataStore.h"

MarketManager::MarketManager() {
    // DataStore is a singleton — already initialized in main.cpp
}

// ── Authentication ────────────────────────────────────────────────────────────

User* MarketManager::login(const QString& name) {
    return DataStore::instance().findUser(name);
}

// ── Market Schedule ───────────────────────────────────────────────────────────

QList<MarketDate*> MarketManager::getFourWeekSchedule() {
    QList<MarketDate*> result;
    for (MarketDate& md : DataStore::instance().getMarketDates()) {
        result.append(&md);
    }
    return result;
}

// ── Booking ───────────────────────────────────────────────────────────────────

bool MarketManager::bookStall(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    // Controller-layer compliance gate — DataStore also checks, but
    // checking here lets the UI surface a specific error message.
    if (!vendor->hasAllRequiredDocs()) return false;
    return DataStore::instance().bookStall(vendor->getId(), date->getId());
}

bool MarketManager::cancelBooking(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    return DataStore::instance().cancelBooking(vendor->getId(), date->getId());
}

bool MarketManager::hasBooking(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    return DataStore::instance().vendorHasBooking(vendor->getId(), date->getId());
}

bool MarketManager::hasActiveBooking(Vendor* vendor) {
    if (!vendor) return false;
    return !DataStore::instance().getBookingsForVendor(vendor->getId()).isEmpty();
}

// ── Waitlist ──────────────────────────────────────────────────────────────────

bool MarketManager::joinWaitlist(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    if (!vendor->hasAllRequiredDocs()) return false;
    return DataStore::instance().joinWaitlist(vendor->getId(), date->getId());
}

bool MarketManager::leaveWaitlist(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    return DataStore::instance().leaveWaitlist(vendor->getId(), date->getId());
}

int MarketManager::getWaitlistPosition(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return -1;
    return DataStore::instance().getWaitlistPosition(vendor->getId(), date->getId());
}

bool MarketManager::isOnWaitlist(Vendor* vendor, MarketDate* date) {
    if (!vendor || !date) return false;
    return DataStore::instance().vendorOnWaitlist(vendor->getId(), date->getId());
}

// ── Vendor Data ───────────────────────────────────────────────────────────────

QList<Booking> MarketManager::getBookingsForVendor(Vendor* vendor) {
    if (!vendor) return {};
    return DataStore::instance().getBookingsForVendor(vendor->getId());
}

QList<WaitlistEntry> MarketManager::getWaitlistForVendor(Vendor* vendor) {
    if (!vendor) return {};
    return DataStore::instance().getWaitlistForVendor(vendor->getId());
}

bool MarketManager::vendorIsCompliant(Vendor* vendor) {
    if (!vendor) return false;
    return vendor->hasAllRequiredDocs();
}

QStringList MarketManager::getMissingDocs(Vendor* vendor) {
    if (!vendor) return {};
    return vendor->getMissingDocs();
}
