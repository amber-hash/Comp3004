#pragma once
#include <QString>
#include <QDate>

enum class DocType { BusinessLicence, LiabilityInsurance, FoodHandlerCert };

class ComplianceDoc {
public:
    ComplianceDoc() = default;
    ComplianceDoc(DocType type, const QString& docNumber, const QDate& expiry,
                  const QString& provider = "");

    DocType getType() const { return m_type; }
    QString getDocNumber() const { return m_docNumber; }
    QDate getExpiry() const { return m_expiry; }
    QString getProvider() const { return m_provider; }
    QString getTypeString() const;
    bool isValidForSeason() const;  // valid past last Sunday of September

private:
    DocType m_type;
    QString m_docNumber;
    QDate m_expiry;
    QString m_provider;  // insurance provider name (for liability insurance)
};
