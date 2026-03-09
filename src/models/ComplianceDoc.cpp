#include "ComplianceDoc.h"
#include <QDate>

ComplianceDoc::ComplianceDoc(DocType type, const QString& docNumber,
                              const QDate& expiry, const QString& provider)
    : m_type(type), m_docNumber(docNumber), m_expiry(expiry), m_provider(provider) {}

QString ComplianceDoc::getTypeString() const {
    switch (m_type) {
        case DocType::BusinessLicence: return "City of Hintonville Business Licence";
        case DocType::LiabilityInsurance: return "Liability Insurance ($2M+)";
        case DocType::FoodHandlerCert: return "Ontario Food Handler Certification";
    }
    return "Unknown";
}

bool ComplianceDoc::isValidForSeason() const {
    // Find last Sunday of September in current year
    int year = QDate::currentDate().year();
    QDate lastSept(year, 9, 30);
    while (lastSept.dayOfWeek() != Qt::Sunday) {
        lastSept = lastSept.addDays(-1);
    }
    return m_expiry > lastSept;
}
