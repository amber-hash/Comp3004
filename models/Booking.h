#pragma once
#include <QString>
#include <QDate>

class Booking {
public:
    Booking() = default;
    Booking(int id, int vendorId, int marketDateId, const QDate& marketDate);
    // Used when loading from DB — confirmation number already exists
    Booking(int id, int vendorId, int marketDateId, const QDate& marketDate, const QString& confirmationNumber);

    int getId() const { return m_id; }
    int getVendorId() const { return m_vendorId; }
    int getMarketDateId() const { return m_marketDateId; }
    QDate getMarketDate() const { return m_marketDate; }
    QString getMarketDateString() const { return m_marketDate.toString("MMMM d, yyyy"); }
    QString getConfirmationNumber() const { return m_confirmationNumber; }

private:
    int m_id;
    int m_vendorId;
    int m_marketDateId;
    QDate m_marketDate;
    QString m_confirmationNumber;
};
