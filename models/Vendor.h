#pragma once
#include "User.h"
#include "ComplianceDoc.h"
#include <QList>
#include <QMap>

enum class VendorCategory { Food, Artisan };

class Vendor : public User {
public:
    Vendor() = default;
    Vendor(int id, const QString& username, const QString& displayName,
           const QString& businessName, const QString& ownerName,
           const QString& email, const QString& phone,
           const QString& address, VendorCategory category);

    QString getBusinessName() const { return m_businessName; }
    QString getOwnerName() const { return m_ownerName; }
    QString getEmail() const { return m_email; }
    QString getPhone() const { return m_phone; }
    QString getAddress() const { return m_address; }
    VendorCategory getCategory() const { return m_category; }
    QString getCategoryString() const;

    // Compliance docs
    void addComplianceDoc(const ComplianceDoc& doc);
    QList<ComplianceDoc> getComplianceDocs() const { return m_complianceDocs; }
    bool hasAllRequiredDocs() const;
    QStringList getMissingDocs() const;

    // Notifications
    void addNotification(const QString& msg);
    QStringList getNotifications() const { return m_notifications; }
    void clearNotifications() { m_notifications.clear(); }

private:
    QString m_businessName;
    QString m_ownerName;
    QString m_email;
    QString m_phone;
    QString m_address;
    VendorCategory m_category;
    QList<ComplianceDoc> m_complianceDocs;
    QStringList m_notifications;
};
