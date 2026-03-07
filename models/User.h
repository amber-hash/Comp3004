#pragma once
#include <QString>

enum class UserType { Vendor, MarketOperator, SystemAdmin };

class User {
public:
    User() = default;
    User(int id, const QString& username, const QString& displayName, UserType type);

    int getId() const { return m_id; }
    QString getUsername() const { return m_username; }
    QString getDisplayName() const { return m_displayName; }
    UserType getUserType() const { return m_type; }
    QString getUserTypeString() const;

protected:
    int m_id;
    QString m_username;
    QString m_displayName;
    UserType m_type;
};
