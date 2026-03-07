#pragma once
#include <QList>
#include <QMap>
#include <QString>
#include <QSqlDatabase>
#include "../models/User.h"
#include "../models/Vendor.h"
#include "../models/MarketDate.h"
#include "../models/Booking.h"
#include "../models/WaitlistEntry.h"

class DataStore {
public:
    static DataStore& instance();

    void initialize();  // load default data

    // User lookup
    User* findUser(const QString& username);
    Vendor* findVendor(const QString& username);
    Vendor* findVendorById(int id);

    // Market schedule
    QList<MarketDate>& getMarketDates() { return m_marketDates; }
    MarketDate* findMarketDate(int id);

    // Bookings
    QList<Booking> getBookingsForVendor(int vendorId) const;
    bool bookStall(int vendorId, int marketDateId);
    bool cancelBooking(int vendorId, int marketDateId);
    bool vendorHasBooking(int vendorId, int marketDateId) const;

    // Waitlist
    QList<WaitlistEntry> getWaitlistForVendor(int vendorId) const;
    bool joinWaitlist(int vendorId, int marketDateId);
    bool leaveWaitlist(int vendorId, int marketDateId);
    bool vendorOnWaitlist(int vendorId, int marketDateId) const;
    int getWaitlistPosition(int vendorId, int marketDateId) const;
    void processWaitlistOnCancellation(int marketDateId, VendorCategory category);

    // All vendors (for operator/admin views)
    QList<Vendor>& getVendors() { return m_vendors; }
    QList<User>& getOperatorsAndAdmins() { return m_staff; }

private:
    DataStore() = default;
    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;

    // Load from SQLite into in-memory lists on startup
    void loadAll();
    void loadUsers();
    void loadMarketDates();
    void loadBookings();
    void loadWaitlist();
    void loadNotifications();

    // Write helpers (called inside transactions)
    void persistNotification(QSqlDatabase& db, int vendorId,
                             const QString& message, const QString& now);
    void writeAuditLog(QSqlDatabase& db, int userId, const QString& action,
                       const QString& details, const QString& now);

    QList<Vendor> m_vendors;
    QList<User> m_staff;
    QList<MarketDate> m_marketDates;
    QList<Booking> m_bookings;
    QList<WaitlistEntry> m_waitlist;

    int m_nextBookingId = 1;
    int m_nextWaitlistId = 1;
};
