#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "../models/Vendor.h"
#include "../controller/MarketManager.h"

class VendorDashboard : public QWidget {
    Q_OBJECT
public:
    explicit VendorDashboard(Vendor* vendor, MarketManager* manager, QWidget* parent = nullptr);
    void refresh();

signals:
    void goToMarketSchedule();
    void signOut();

private:
    void setupUI();
    void buildBusinessInfo(QVBoxLayout* layout);
    void buildComplianceDocs(QVBoxLayout* layout);
    void buildBookings(QVBoxLayout* layout);
    void buildWaitlist(QVBoxLayout* layout);
    void buildNotifications(QVBoxLayout* layout);

    Vendor*        m_vendor;
    MarketManager* m_manager;   // All data calls go through here
    QVBoxLayout*   m_mainLayout = nullptr;
};
