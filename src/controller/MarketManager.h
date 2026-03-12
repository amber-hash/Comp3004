#pragma once
#include <QString>
#include <QList>
#include "../models/User.h"
#include "../models/Vendor.h"
#include "../models/MarketDate.h"
#include "../models/Booking.h"
#include "../models/WaitlistEntry.h"

/**
 * MarketManager — Controller layer
 *
 * This is the ONLY class the UI layer talks to.
 * It sits between the boundary classes (UI) and the DataStore (data layer).
 *
 * UI Layer   →   MarketManager   →   DataStore
 * (Boundary)     (Controller)        (Entity data)
 */
class MarketManager {
public:
    MarketManager();

    // ── Authentication ──────────────────────────────────────────────────────
    // Looks up a user by username or display name. Returns nullptr if not found.
    User* login(const QString& name);

    // ── Market Schedule ─────────────────────────────────────────────────────
    // Returns the 4 upcoming market dates available for booking.
    QList<MarketDate*> getFourWeekSchedule();

    // ── Booking ─────────────────────────────────────────────────────────────
    // Attempts to book a stall for the vendor on the given market date.
    // Returns true on success, false if unavailable or rules violated.
    bool bookStall(Vendor* vendor, MarketDate* date);

    // Cancels an existing booking. Returns true on success.
    bool cancelBooking(Vendor* vendor, MarketDate* date);

    // Returns true if the vendor already has a booking on this date.
    bool hasBooking(Vendor* vendor, MarketDate* date);

    // Returns true if the vendor has ANY active booking (enforces one-at-a-time rule).
    bool hasActiveBooking(Vendor* vendor);

    // ── Waitlist ─────────────────────────────────────────────────────────────
    // Adds vendor to the waitlist for this date. Returns true if added.
    bool joinWaitlist(Vendor* vendor, MarketDate* date);

    // Removes vendor from the waitlist for this date. Returns true if removed.
    bool leaveWaitlist(Vendor* vendor, MarketDate* date);

    // Returns vendor's 1-based queue position, or -1 if not on the waitlist.
    int getWaitlistPosition(Vendor* vendor, MarketDate* date);

    // Returns true if the vendor is on the waitlist for this date.
    bool isOnWaitlist(Vendor* vendor, MarketDate* date);

    // Returns true if the vendor gets notified on waitlist
    bool isWaitlistNotified(Vendor* vendor, MarketDate* date);

    // ── Vendor Data ──────────────────────────────────────────────────────────
    // Returns all bookings for a vendor.
    QList<Booking> getBookingsForVendor(Vendor* vendor);

    // Returns all waitlist entries for a vendor.
    QList<WaitlistEntry> getWaitlistForVendor(Vendor* vendor);

    // Returns true if vendor has all required compliance docs.
    bool vendorIsCompliant(Vendor* vendor);

    // Returns list of missing/expired compliance document names.
    QStringList getMissingDocs(Vendor* vendor);
};
