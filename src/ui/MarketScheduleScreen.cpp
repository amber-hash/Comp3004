#include "MarketScheduleScreen.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidgetItem>

MarketScheduleScreen::MarketScheduleScreen(Vendor* vendor, MarketManager* manager, QWidget* parent)
    : QWidget(parent), m_vendor(vendor), m_manager(manager) {
    setupUI();
}

// ── Helper ────────────────────────────────────────────────────────────────────
MarketDate* MarketScheduleScreen::getSelectedDate() {
    int row = m_table->currentRow();
    if (row < 0) return nullptr;
    QTableWidgetItem* item = m_table->item(row, 0);
    if (!item) return nullptr;
    // We stored the MarketDate pointer in UserRole
    return static_cast<MarketDate*>(item->data(Qt::UserRole).value<void*>());
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void MarketScheduleScreen::setupUI() {
    setStyleSheet(StyleSheet::global());

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ── Toolbar ──────────────────────────────────────────────────────────────
    QFrame* toolbar = new QFrame();
    toolbar->setStyleSheet("QFrame { background: #3D6B4F; border: none; }");
    toolbar->setFixedHeight(56);
    QHBoxLayout* tbl = new QHBoxLayout(toolbar);
    tbl->setContentsMargins(20, 0, 20, 0);

    QLabel* appName = new QLabel("🌿 HintonMarket");
    appName->setStyleSheet("color: #F8F3E8; font-size: 18px; font-weight: bold; font-family: Georgia, serif;");
    QWidget* spacer = new QWidget(); spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QPushButton* dashBtn = new QPushButton("← My Dashboard");
    dashBtn->setStyleSheet(R"(
        QPushButton { background: #4E8563; color: #F8F3E8; border: none; border-radius: 4px;
                      padding: 6px 14px; font-family: Georgia, serif; font-size: 12px; }
        QPushButton:hover { background: #5D9B74; }
    )");
    dashBtn->setCursor(Qt::PointingHandCursor);
    connect(dashBtn, &QPushButton::clicked, this, &MarketScheduleScreen::goToDashboard);

    QPushButton* signOutBtn = new QPushButton("Sign Out");
    signOutBtn->setStyleSheet(R"(
        QPushButton { background: transparent; color: #F8F3E8; border: 1px solid #C8E6D0;
                      border-radius: 4px; padding: 6px 14px; font-family: Georgia, serif; font-size: 12px; }
        QPushButton:hover { background: rgba(255,255,255,0.1); }
    )");
    signOutBtn->setCursor(Qt::PointingHandCursor);
    connect(signOutBtn, &QPushButton::clicked, this, &MarketScheduleScreen::signOut);

    tbl->addWidget(appName); tbl->addWidget(spacer);
    tbl->addWidget(dashBtn); tbl->addSpacing(8); tbl->addWidget(signOutBtn);
    outerLayout->addWidget(toolbar);

    // ── Content ───────────────────────────────────────────────────────────────
    QWidget* content = new QWidget();
    content->setStyleSheet("background: #FDFAF4;");
    QVBoxLayout* cl = new QVBoxLayout(content);
    cl->setContentsMargins(32, 28, 32, 24);
    cl->setSpacing(16);

    QLabel* pageTitle = new QLabel("Market Schedule");
    pageTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #2B2016; font-family: Georgia, serif;");
    QLabel* pageSub = new QLabel(
        QString("Upcoming market dates for %1 vendors  •  Max 2 stalls per category per day")
        .arg(m_vendor->getCategoryString()));
    pageSub->setStyleSheet("font-size: 13px; color: #7A5C3A;");
    cl->addWidget(pageTitle);
    cl->addWidget(pageSub);

    // Compliance warning — uses manager->vendorIsCompliant()
    if (!m_manager->vendorIsCompliant(m_vendor)) {
        QFrame* alert = new QFrame();
        alert->setStyleSheet("QFrame { background: #FFF3CD; border: 1px solid #F0C040; border-radius: 6px; }");
        QHBoxLayout* al = new QHBoxLayout(alert);
        al->setContentsMargins(14, 10, 14, 10);
        QLabel* alertLbl = new QLabel("⚠️  Booking disabled — compliance documents incomplete. Update your profile to enable booking.");
        alertLbl->setStyleSheet("color: #7A5C10; font-size: 12px;");
        alertLbl->setWordWrap(true);
        al->addWidget(alertLbl);
        cl->addWidget(alert);
    }

    // Legend
    QFrame* legend = new QFrame();
    legend->setStyleSheet("QFrame { background: #F5F0E8; border: 1px solid #D9CDB8; border-radius: 6px; }");
    QHBoxLayout* ll = new QHBoxLayout(legend);
    ll->setContentsMargins(14, 8, 14, 8); ll->setSpacing(20);
    auto addLegend = [&](const QString& color, const QString& text) {
        QLabel* dot = new QLabel("●"); dot->setStyleSheet(QString("color: %1; font-size: 16px;").arg(color));
        QLabel* lbl = new QLabel(text); lbl->setStyleSheet("font-size: 12px; color: #5C4A30;");
        ll->addWidget(dot); ll->addWidget(lbl);
    };
    QLabel* lt = new QLabel("Status:"); lt->setStyleSheet("font-size: 12px; font-weight: bold; color: #5C4A30;");
    ll->addWidget(lt);
    addLegend("#3D6B4F", "Available");
    addLegend("#C47C20", "Waitlist Only");
    addLegend("#B84040", "Full");
    addLegend("#2060C0", "Your Booking");
    addLegend("#888888", "1 Booking Limit");
    ll->addStretch();
    cl->addWidget(legend);

    // ── Schedule Table ────────────────────────────────────────────────────────
    m_table = new QTableWidget();
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({
        "Market Date",
        m_vendor->getCategoryString() + " Availability",
        "Status", "Your Booking", "Waitlist"
    });
    m_table->setStyleSheet(StyleSheet::tableWidget() + "QTableWidget { alternate-background-color: #F5F0E8; }");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &MarketScheduleScreen::onSelectionChanged);
    populateTable();
    cl->addWidget(m_table);

    // ── Action Bar ────────────────────────────────────────────────────────────
    QFrame* actionBar = new QFrame();
    actionBar->setStyleSheet("QFrame { background: #FAF7F0; border: 1px solid #D9CDB8; border-radius: 6px; }");
    QHBoxLayout* al = new QHBoxLayout(actionBar);
    al->setContentsMargins(16, 12, 16, 12); al->setSpacing(12);

    QLabel* actionLabel = new QLabel("Select a row to take action:");
    actionLabel->setStyleSheet("color: #7A5C3A; font-size: 12px;");

    m_bookBtn = new QPushButton("📋  Book Stall");
    m_bookBtn->setStyleSheet(StyleSheet::primaryButton());
    m_bookBtn->setCursor(Qt::PointingHandCursor);
    m_bookBtn->setEnabled(false);
    connect(m_bookBtn, &QPushButton::clicked, this, &MarketScheduleScreen::onBookSelected);

    m_waitlistBtn = new QPushButton("⏳  Join Waitlist");
    m_waitlistBtn->setStyleSheet(StyleSheet::secondaryButton());
    m_waitlistBtn->setCursor(Qt::PointingHandCursor);
    m_waitlistBtn->setEnabled(false);
    connect(m_waitlistBtn, &QPushButton::clicked, this, &MarketScheduleScreen::onJoinWaitlistSelected);

    m_cancelBookingBtn = new QPushButton("✗  Cancel Booking");
    m_cancelBookingBtn->setStyleSheet(StyleSheet::dangerButton());
    m_cancelBookingBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBookingBtn->setEnabled(false);
    connect(m_cancelBookingBtn, &QPushButton::clicked, this, &MarketScheduleScreen::onCancelBookingSelected);

    m_leaveWaitlistBtn = new QPushButton("✗  Leave Waitlist");
    m_leaveWaitlistBtn->setStyleSheet(StyleSheet::dangerButton());
    m_leaveWaitlistBtn->setCursor(Qt::PointingHandCursor);
    m_leaveWaitlistBtn->setEnabled(false);
    connect(m_leaveWaitlistBtn, &QPushButton::clicked, this, &MarketScheduleScreen::onLeaveWaitlistSelected);

    al->addWidget(actionLabel); al->addStretch();
    al->addWidget(m_bookBtn); al->addWidget(m_waitlistBtn);
    al->addWidget(m_cancelBookingBtn); al->addWidget(m_leaveWaitlistBtn);
    cl->addWidget(actionBar);

    m_statusBar = new QLabel("");
    m_statusBar->setStyleSheet("font-size: 12px; color: #5C4A30; padding: 4px 0;");
    cl->addWidget(m_statusBar);

    outerLayout->addWidget(content);
}

// ── Table Population ──────────────────────────────────────────────────────────
void MarketScheduleScreen::populateTable() {
    // Uses manager->getFourWeekSchedule() — the agreed API call
    QList<MarketDate*> dates = m_manager->getFourWeekSchedule();
    m_table->setRowCount(dates.size());

    bool vendorHasAnyBooking = m_manager->hasActiveBooking(m_vendor);

    for (int i = 0; i < dates.size(); ++i) {
        MarketDate* md = dates[i];
        bool isFood   = m_vendor->getCategory() == VendorCategory::Food;
        int  avail    = isFood ? md->getAvailableFood() : md->getAvailableArtisan();
        int  total    = isFood ? md->getMaxFood()       : md->getMaxArtisan();

        // Uses manager->hasBooking() and manager->isOnWaitlist()
        bool hasBooking  = m_manager->hasBooking(m_vendor, md);
        bool onWaitlist  = m_manager->isOnWaitlist(m_vendor, md);
        int  waitlistPos = m_manager->getWaitlistPosition(m_vendor, md);
        // This row is blocked if vendor already booked a different date
        bool blockedByOther = vendorHasAnyBooking && !hasBooking;

        // Col 0: Date — store the MarketDate pointer for later retrieval
        QTableWidgetItem* dateItem = new QTableWidgetItem("  " + md->getDateString());
        dateItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(md)));
        m_table->setItem(i, 0, dateItem);

        // Col 1: Availability
        QTableWidgetItem* availItem = new QTableWidgetItem(
            QString("%1 / %2 stalls available").arg(avail).arg(total));
        availItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 1, availItem);

        // Col 2: Status
        QString statusText;
        QColor  statusColor;
        if (hasBooking) {
            statusText  = "Your Booking";
            statusColor = QColor("#2060C0");
        } else if (blockedByOther) {
            statusText  = "1 Booking Limit";
            statusColor = QColor("#888888");
        } else if (avail > 0) {
            statusText  = "Available";
            statusColor = QColor("#3D6B4F");
        } else {
            statusText  = "Waitlist Only";
            statusColor = QColor("#C47C20");
        }
        QTableWidgetItem* statusItem = new QTableWidgetItem(statusText);
        statusItem->setForeground(statusColor);
        statusItem->setFont(QFont("Georgia", 12, QFont::Bold));
        statusItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, statusItem);

        // Col 3: Your booking
        QTableWidgetItem* bookItem = new QTableWidgetItem(hasBooking ? "✅  Booked" : "—");
        bookItem->setTextAlignment(Qt::AlignCenter);
        if (hasBooking) bookItem->setForeground(QColor("#2060C0"));
        m_table->setItem(i, 3, bookItem);

        // Col 4: Waitlist position
        QString waitText = onWaitlist ? QString("Position #%1").arg(waitlistPos) : "—";
        QTableWidgetItem* waitItem = new QTableWidgetItem(waitText);
        waitItem->setTextAlignment(Qt::AlignCenter);
        if (onWaitlist) waitItem->setForeground(QColor("#C47C20"));
        m_table->setItem(i, 4, waitItem);

        m_table->setRowHeight(i, 42);
    }
}

// ── Action Button State ───────────────────────────────────────────────────────
void MarketScheduleScreen::onSelectionChanged() {
    updateActionButtons();
}

void MarketScheduleScreen::updateActionButtons() {
    MarketDate* md = getSelectedDate();

    if (!md) {
        m_bookBtn->setEnabled(false);
        m_waitlistBtn->setEnabled(false);
        m_cancelBookingBtn->setEnabled(false);
        m_leaveWaitlistBtn->setEnabled(false);
        m_statusBar->setText("");
        return;
    }

    bool isFood         = m_vendor->getCategory() == VendorCategory::Food;
    int  avail          = isFood ? md->getAvailableFood() : md->getAvailableArtisan();
    bool hasBooking     = m_manager->hasBooking(m_vendor, md);
    bool onWaitlist     = m_manager->isOnWaitlist(m_vendor, md);
    bool isCompliant    = m_manager->vendorIsCompliant(m_vendor);
    // One-at-a-time rule: already booked a different date
    bool hasOtherBooking = !hasBooking && m_manager->hasActiveBooking(m_vendor);

    // Book: compliant, stall available, no booking here, no booking elsewhere, not on waitlist
    m_bookBtn->setEnabled(isCompliant && avail > 0 && !hasBooking && !hasOtherBooking && !onWaitlist);
    // Waitlist: compliant, no stalls, no booking here, not already on this waitlist
    m_waitlistBtn->setEnabled(isCompliant && avail == 0 && !hasBooking && !onWaitlist);
    m_cancelBookingBtn->setEnabled(hasBooking);
    m_leaveWaitlistBtn->setEnabled(onWaitlist);

    QString status;
    if (!isCompliant)
        status = "⚠  Complete your compliance documents to enable booking.";
    else if (hasBooking)
        status = "✅  You have a booking for this date. You may cancel it.";
    else if (hasOtherBooking)
        status = "ℹ  You already have an active booking on another date. Cancel it first to book here.";
    else if (onWaitlist) {
        int pos = m_manager->getWaitlistPosition(m_vendor, md);
        status = QString("⏳  You are #%1 on the waitlist for this date.").arg(pos);
    }
    else if (avail > 0)
        status = QString("📋  %1 stall(s) available. Click \"Book Stall\" to reserve.").arg(avail);
    else
        status = "⏳  No stalls available. You may join the waitlist.";

    m_statusBar->setText(status);
}

// ── Actions — each calls the matching MarketManager method ───────────────────

void MarketScheduleScreen::onBookSelected() {
    MarketDate* md = getSelectedDate();
    if (!md) return;

    QMessageBox confirm(this);
    confirm.setWindowTitle("Confirm Booking");
    confirm.setText("<b>Confirm Stall Booking</b>");
    confirm.setInformativeText(
        QString("Book a %1 vendor stall for:<br><b>%2</b>")
        .arg(m_vendor->getCategoryString()).arg(md->getDateString()));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setStyleSheet(StyleSheet::global());

    if (confirm.exec() == QMessageBox::Yes) {
        // Delegate to manager->bookStall()
        if (m_manager->bookStall(m_vendor, md)) {
            QMessageBox::information(this, "Booking Confirmed",
                QString("✅ Your stall has been booked for <b>%1</b>.<br><br>"
                        "Check your dashboard for confirmation details.")
                .arg(md->getDateString()));
        } else {
            // Determine reason for failure to show the right message
            QString reason;
            if (!m_manager->vendorIsCompliant(m_vendor))
                reason = "Your compliance documents are incomplete. Please update your profile.";
            else if (m_manager->hasActiveBooking(m_vendor))
                reason = "You already have an active booking on another date.<br>"
                         "Cancel that booking first to book a new stall.";
            else
                reason = "The stall is no longer available for this date.";
            QMessageBox::warning(this, "Booking Failed", reason);
        }
        refresh();
    }
}

void MarketScheduleScreen::onJoinWaitlistSelected() {
    MarketDate* md = getSelectedDate();
    if (!md) return;

    QMessageBox confirm(this);
    confirm.setWindowTitle("Join Waitlist");
    confirm.setText("<b>Join Stall Waitlist</b>");
    confirm.setInformativeText(
        QString("No %1 stalls available for <b>%2</b>.<br><br>"
                "Would you like to join the waitlist?")
        .arg(m_vendor->getCategoryString()).arg(md->getDateString()));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setStyleSheet(StyleSheet::global());

    if (confirm.exec() == QMessageBox::Yes) {
        // Delegate to manager->joinWaitlist()
        if (m_manager->joinWaitlist(m_vendor, md)) {
            int pos = m_manager->getWaitlistPosition(m_vendor, md);
            QMessageBox::information(this, "Added to Waitlist",
                QString("⏳ Added to waitlist for <b>%1</b>.<br>Your position: <b>#%2</b>")
                .arg(md->getDateString()).arg(pos));
        } else {
            QMessageBox::warning(this, "Error", "Unable to join waitlist.");
        }
        refresh();
    }
}

void MarketScheduleScreen::onCancelBookingSelected() {
    MarketDate* md = getSelectedDate();
    if (!md) return;

    QMessageBox confirm(this);
    confirm.setWindowTitle("Cancel Booking");
    confirm.setText("<b>Cancel Stall Booking?</b>");
    confirm.setInformativeText(
        QString("Cancel your booking for <b>%1</b>?<br>This cannot be undone.")
        .arg(md->getDateString()));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.setStyleSheet(StyleSheet::global());

    if (confirm.exec() == QMessageBox::Yes) {
        // Delegate to manager->cancelBooking()
        if (m_manager->cancelBooking(m_vendor, md)) {
            QMessageBox::information(this, "Booking Cancelled",
                QString("❌ Booking for <b>%1</b> has been cancelled.")
                .arg(md->getDateString()));
        } else {
            QMessageBox::warning(this, "Error", "Unable to cancel booking.");
        }
        refresh();
    }
}

void MarketScheduleScreen::onLeaveWaitlistSelected() {
    MarketDate* md = getSelectedDate();
    if (!md) return;

    QMessageBox confirm(this);
    confirm.setWindowTitle("Leave Waitlist");
    confirm.setText("<b>Leave Waitlist?</b>");
    confirm.setInformativeText(
        QString("Remove yourself from the waitlist for <b>%1</b>?")
        .arg(md->getDateString()));
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    confirm.setStyleSheet(StyleSheet::global());

    if (confirm.exec() == QMessageBox::Yes) {
        // Delegate to manager->leaveWaitlist()
        if (m_manager->leaveWaitlist(m_vendor, md)) {
            QMessageBox::information(this, "Removed from Waitlist",
                QString("🚫 Removed from waitlist for <b>%1</b>.").arg(md->getDateString()));
        } else {
            QMessageBox::warning(this, "Error", "Unable to leave waitlist.");
        }
        refresh();
    }
}

void MarketScheduleScreen::refresh() {
    populateTable();
    updateActionButtons();
}
