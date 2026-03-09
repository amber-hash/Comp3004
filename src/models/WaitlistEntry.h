#pragma once
#include <QString>
#include <QDate>
#include "Vendor.h"

class WaitlistEntry {
public:
    WaitlistEntry() = default;
    WaitlistEntry(int id, int vendorId, int marketDateId, const QDate& marketDate,
                  VendorCategory category);

    int getId() const { return m_id; }
    int getVendorId() const { return m_vendorId; }
    int getMarketDateId() const { return m_marketDateId; }
    QDate getMarketDate() const { return m_marketDate; }
    QString getMarketDateString() const { return m_marketDate.toString("MMMM d, yyyy"); }
    VendorCategory getCategory() const { return m_category; }
    int getPosition() const { return m_position; }
    void setPosition(int pos) { m_position = pos; }
    bool isNotified() const { return m_notified; }
    void setNotified(bool n) { m_notified = n; }

private:
    int m_id;
    int m_vendorId;
    int m_marketDateId;
    QDate m_marketDate;
    VendorCategory m_category;
    int m_position = 0;
    bool m_notified = false;
};
