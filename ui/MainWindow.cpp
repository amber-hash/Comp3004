#include "MainWindow.h"
#include "LoginScreen.h"
#include "VendorDashboard.h"
#include "MarketScheduleScreen.h"
#include "StyleSheet.h"
#include "../models/Vendor.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("HintonMarket — Hintonville Farmers Market Management System");
    resize(1100, 760);
    setMinimumSize(900, 600);

    // Create the single controller instance.
    // This is the ONLY place MarketManager is constructed.
    // All screens receive a pointer to it — none construct their own.
    m_manager = new MarketManager();

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // LoginScreen gets the manager so it can call login()
    m_loginScreen = new LoginScreen(m_manager);
    connect(m_loginScreen, &LoginScreen::loginSuccess, this, &MainWindow::onLoginSuccess);
    m_stack->addWidget(m_loginScreen);

    m_stack->setCurrentWidget(m_loginScreen);
}

MainWindow::~MainWindow() {
    delete m_manager;
}

void MainWindow::onLoginSuccess(User* user) {
    m_currentUser = user;

    if (user->getUserType() == UserType::Vendor) {
        Vendor* vendor = static_cast<Vendor*>(user);

        // Tear down old screens so we start fresh for new user
        if (m_vendorDashboard) {
            m_stack->removeWidget(m_vendorDashboard);
            delete m_vendorDashboard;
            m_vendorDashboard = nullptr;
        }
        if (m_marketSchedule) {
            m_stack->removeWidget(m_marketSchedule);
            delete m_marketSchedule;
            m_marketSchedule = nullptr;
        }

        // Both screens receive the shared manager — no direct DataStore access
        m_vendorDashboard = new VendorDashboard(vendor, m_manager);
        connect(m_vendorDashboard, &VendorDashboard::goToMarketSchedule,
                this, &MainWindow::showMarketSchedule);
        connect(m_vendorDashboard, &VendorDashboard::signOut,
                this, &MainWindow::onSignOut);
        m_stack->addWidget(m_vendorDashboard);

        m_marketSchedule = new MarketScheduleScreen(vendor, m_manager);
        connect(m_marketSchedule, &MarketScheduleScreen::goToDashboard,
                this, &MainWindow::showVendorDashboard);
        connect(m_marketSchedule, &MarketScheduleScreen::signOut,
                this, &MainWindow::onSignOut);
        m_stack->addWidget(m_marketSchedule);

        m_stack->setCurrentWidget(m_vendorDashboard);

    } else {
        QMessageBox::information(this,
            "Welcome — " + user->getUserTypeString(),
            QString("Welcome, %1!\n\nThe Market Operator and System Administrator interfaces\nare implemented in the full system.\n\nFor this prototype, please sign in with a Vendor account.")
            .arg(user->getDisplayName()));
        m_stack->setCurrentWidget(m_loginScreen);
    }
}

void MainWindow::onSignOut() {
    // Clear vendor notifications so they don't bleed into the next login session
    if (m_currentUser && m_currentUser->getUserType() == UserType::Vendor) {
        static_cast<Vendor*>(m_currentUser)->clearNotifications();
    }
    m_currentUser = nullptr;
    if (m_vendorDashboard) {
        m_stack->removeWidget(m_vendorDashboard);
        delete m_vendorDashboard;
        m_vendorDashboard = nullptr;
    }
    if (m_marketSchedule) {
        m_stack->removeWidget(m_marketSchedule);
        delete m_marketSchedule;
        m_marketSchedule = nullptr;
    }
    m_stack->setCurrentWidget(m_loginScreen);
}

void MainWindow::showVendorDashboard() {
    if (m_vendorDashboard) {
        m_vendorDashboard->refresh();
        m_stack->setCurrentWidget(m_vendorDashboard);
    }
}

void MainWindow::showMarketSchedule() {
    if (m_marketSchedule) {
        m_marketSchedule->refresh();
        m_stack->setCurrentWidget(m_marketSchedule);
    }
}
