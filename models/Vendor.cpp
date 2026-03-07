#include "Vendor.h"

Vendor::Vendor(int id, const QString& username, const QString& displayName,
               const QString& businessName, const QString& ownerName,
               const QString& email, const QString& phone,
               const QString& address, VendorCategory category)
    : User(id, username, displayName, UserType::Vendor),
      m_businessName(businessName), m_ownerName(ownerName),
      m_email(email), m_phone(phone), m_address(address),
      m_category(category) {}

QString Vendor::getCategoryString() const {
    return m_category == VendorCategory::Food ? "Food" : "Artisan";
}

void Vendor::addComplianceDoc(const ComplianceDoc& doc) {
    // Replace if same type exists
    for (int i = 0; i < m_complianceDocs.size(); ++i) {
        if (m_complianceDocs[i].getType() == doc.getType()) {
            m_complianceDocs[i] = doc;
            return;
        }
    }
    m_complianceDocs.append(doc);
}

bool Vendor::hasAllRequiredDocs() const {
    return getMissingDocs().isEmpty();
}

QStringList Vendor::getMissingDocs() const {
    QStringList missing;
    bool hasLicence = false, hasInsurance = false, hasFoodCert = false;
    for (const auto& doc : m_complianceDocs) {
        if (doc.getType() == DocType::BusinessLicence && doc.isValidForSeason()) hasLicence = true;
        if (doc.getType() == DocType::LiabilityInsurance && doc.isValidForSeason()) hasInsurance = true;
        if (doc.getType() == DocType::FoodHandlerCert && doc.isValidForSeason()) hasFoodCert = true;
    }
    if (!hasLicence) missing << "City of Hintonville Business Licence";
    if (!hasInsurance) missing << "Liability Insurance ($2M+ coverage)";
    if (m_category == VendorCategory::Food && !hasFoodCert)
        missing << "Ontario Food Handler Certification";
    return missing;
}

void Vendor::addNotification(const QString& msg) {
    m_notifications.prepend(msg);  // newest first
}
