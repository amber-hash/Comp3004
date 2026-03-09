#include "User.h"

User::User(int id, const QString& username, const QString& displayName, UserType type)
    : m_id(id), m_username(username), m_displayName(displayName), m_type(type) {}

QString User::getUserTypeString() const {
    switch (m_type) {
        case UserType::Vendor: return "Vendor";
        case UserType::MarketOperator: return "Market Operator";
        case UserType::SystemAdmin: return "System Administrator";
    }
    return "Unknown";
}
