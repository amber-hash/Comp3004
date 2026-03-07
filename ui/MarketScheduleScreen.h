#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include "../models/Vendor.h"
#include "../models/MarketDate.h"
#include "../controller/MarketManager.h"

class MarketScheduleScreen : public QWidget {
    Q_OBJECT
public:
    explicit MarketScheduleScreen(Vendor* vendor, MarketManager* manager, QWidget* parent = nullptr);
    void refresh();

signals:
    void goToDashboard();
    void signOut();

private slots:
    void onBookSelected();
    void onJoinWaitlistSelected();
    void onCancelBookingSelected();
    void onLeaveWaitlistSelected();
    void onSelectionChanged();

private:
    void setupUI();
    void populateTable();
    void updateActionButtons();
    MarketDate* getSelectedDate();   // helper: returns MarketDate* for current row

    Vendor*        m_vendor;
    MarketManager* m_manager;   // All business logic calls go through here

    QTableWidget* m_table;
    QPushButton*  m_bookBtn;
    QPushButton*  m_waitlistBtn;
    QPushButton*  m_cancelBookingBtn;
    QPushButton*  m_leaveWaitlistBtn;
    QLabel*       m_statusBar;
};
