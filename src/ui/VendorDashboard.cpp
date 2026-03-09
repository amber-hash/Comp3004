#include "VendorDashboard.h"
#include "StyleSheet.h"
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>

VendorDashboard::VendorDashboard(Vendor* vendor, MarketManager* manager, QWidget* parent)
    : QWidget(parent), m_vendor(vendor), m_manager(manager) {
    setupUI();
}

static QFrame* makeDivider() {
    QFrame* d = new QFrame();
    d->setFrameShape(QFrame::HLine);
    d->setStyleSheet("background: #D9CDB8; border: none; max-height: 1px; margin: 4px 0;");
    return d;
}

static QLabel* makeSectionTitle(const QString& text) {
    QLabel* lbl = new QLabel(text);
    lbl->setStyleSheet(R"(
        font-size: 15px; font-weight: bold; color: #3D6B4F;
        font-family: "Georgia", serif; padding-bottom: 2px;
    )");
    return lbl;
}

void VendorDashboard::setupUI() {
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

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QLabel* userLabel = new QLabel(m_vendor->getBusinessName() + "  |  " + m_vendor->getCategoryString() + " Vendor");
    userLabel->setStyleSheet("color: #C8E6D0; font-size: 12px;");

    QPushButton* scheduleBtn = new QPushButton("📅  Market Schedule");
    scheduleBtn->setStyleSheet(R"(
        QPushButton { background: #4E8563; color: #F8F3E8; border: none; border-radius: 4px;
                      padding: 6px 14px; font-family: Georgia, serif; font-size: 12px; }
        QPushButton:hover { background: #5D9B74; }
    )");
    scheduleBtn->setCursor(Qt::PointingHandCursor);
    connect(scheduleBtn, &QPushButton::clicked, this, &VendorDashboard::goToMarketSchedule);

    QPushButton* signOutBtn = new QPushButton("Sign Out");
    signOutBtn->setStyleSheet(R"(
        QPushButton { background: transparent; color: #F8F3E8; border: 1px solid #C8E6D0;
                      border-radius: 4px; padding: 6px 14px; font-family: Georgia, serif; font-size: 12px; }
        QPushButton:hover { background: rgba(255,255,255,0.1); }
    )");
    signOutBtn->setCursor(Qt::PointingHandCursor);
    connect(signOutBtn, &QPushButton::clicked, this, &VendorDashboard::signOut);

    tbl->addWidget(appName);
    tbl->addWidget(spacer);
    tbl->addWidget(userLabel);
    tbl->addSpacing(16);
    tbl->addWidget(scheduleBtn);
    tbl->addSpacing(8);
    tbl->addWidget(signOutBtn);
    outerLayout->addWidget(toolbar);

    // ── Scroll area ──────────────────────────────────────────────────────────
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: #FDFAF4; }");

    QWidget* scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: #FDFAF4;");
    m_mainLayout = new QVBoxLayout(scrollContent);
    m_mainLayout->setContentsMargins(32, 28, 32, 32);
    m_mainLayout->setSpacing(24);

    QLabel* pageTitle = new QLabel("Vendor Dashboard");
    pageTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #2B2016; font-family: Georgia, serif;");
    m_mainLayout->addWidget(pageTitle);

    // Compliance alert — uses manager->vendorIsCompliant()
    if (!m_manager->vendorIsCompliant(m_vendor)) {
        QFrame* alertBox = new QFrame();
        alertBox->setStyleSheet("QFrame { background: #FFF3CD; border: 1px solid #F0C040; border-radius: 6px; }");
        QVBoxLayout* abl = new QVBoxLayout(alertBox);
        abl->setContentsMargins(16, 12, 16, 12);
        QLabel* alertTitle = new QLabel("⚠️  Compliance Documents Required");
        alertTitle->setStyleSheet("font-weight: bold; color: #7A5C10;");
        QStringList missing = m_manager->getMissingDocs(m_vendor);
        QLabel* alertMsg = new QLabel("You cannot book stalls until the following are on file:\n• " + missing.join("\n• "));
        alertMsg->setStyleSheet("color: #7A5C10; font-size: 12px;");
        alertMsg->setWordWrap(true);
        abl->addWidget(alertTitle);
        abl->addWidget(alertMsg);
        m_mainLayout->addWidget(alertBox);
    }

    // ── Two-column: business info + compliance docs ───────────────────────
    QFrame* topRow = new QFrame();
    topRow->setStyleSheet("QFrame { background: transparent; border: none; }");
    QHBoxLayout* topRowLayout = new QHBoxLayout(topRow);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(20);

    QFrame* bizCard = new QFrame();
    bizCard->setStyleSheet(StyleSheet::card());
    QVBoxLayout* bizLayout = new QVBoxLayout(bizCard);
    bizLayout->setContentsMargins(20, 16, 20, 16);
    bizLayout->setSpacing(10);
    buildBusinessInfo(bizLayout);

    QFrame* compCard = new QFrame();
    compCard->setStyleSheet(StyleSheet::card());
    QVBoxLayout* compLayout = new QVBoxLayout(compCard);
    compLayout->setContentsMargins(20, 16, 20, 16);
    compLayout->setSpacing(10);
    buildComplianceDocs(compLayout);

    topRowLayout->addWidget(bizCard, 1);
    topRowLayout->addWidget(compCard, 1);
    m_mainLayout->addWidget(topRow);

    // ── Bookings ─────────────────────────────────────────────────────────────
    QFrame* bookCard = new QFrame();
    bookCard->setStyleSheet(StyleSheet::card());
    QVBoxLayout* bookLayout = new QVBoxLayout(bookCard);
    bookLayout->setContentsMargins(20, 16, 20, 16);
    bookLayout->setSpacing(10);
    buildBookings(bookLayout);
    m_mainLayout->addWidget(bookCard);

    // ── Waitlist ──────────────────────────────────────────────────────────────
    QFrame* waitCard = new QFrame();
    waitCard->setStyleSheet(StyleSheet::card());
    QVBoxLayout* waitLayout = new QVBoxLayout(waitCard);
    waitLayout->setContentsMargins(20, 16, 20, 16);
    waitLayout->setSpacing(10);
    buildWaitlist(waitLayout);
    m_mainLayout->addWidget(waitCard);

    // ── Notifications ─────────────────────────────────────────────────────────
    QFrame* notifCard = new QFrame();
    notifCard->setStyleSheet(StyleSheet::card());
    QVBoxLayout* notifLayout = new QVBoxLayout(notifCard);
    notifLayout->setContentsMargins(20, 16, 20, 16);
    notifLayout->setSpacing(10);
    buildNotifications(notifLayout);
    m_mainLayout->addWidget(notifCard);

    m_mainLayout->addStretch();

    scroll->setWidget(scrollContent);
    outerLayout->addWidget(scroll);
}

void VendorDashboard::buildBusinessInfo(QVBoxLayout* layout) {
    layout->addWidget(makeSectionTitle("Business Information"));
    layout->addWidget(makeDivider());

    auto addRow = [&](const QString& label, const QString& value) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel(label + ":");
        lbl->setStyleSheet("color: #7A5C3A; font-size: 12px; min-width: 130px;");
        QLabel* val = new QLabel(value);
        val->setStyleSheet("color: #2B2016; font-size: 13px;");
        val->setWordWrap(true);
        row->addWidget(lbl);
        row->addWidget(val, 1);
        layout->addLayout(row);
    };

    addRow("Business Name", m_vendor->getBusinessName());
    addRow("Owner",         m_vendor->getOwnerName());
    addRow("Email",         m_vendor->getEmail());
    addRow("Phone",         m_vendor->getPhone());
    addRow("Mailing Address", m_vendor->getAddress());

    QHBoxLayout* catRow = new QHBoxLayout();
    QLabel* catLbl = new QLabel("Category:");
    catLbl->setStyleSheet("color: #7A5C3A; font-size: 12px; min-width: 130px;");
    QLabel* catBadge = new QLabel(m_vendor->getCategoryString());
    QString badgeColor = (m_vendor->getCategory() == VendorCategory::Food) ? "#3D6B4F" : "#7A5C3A";
    catBadge->setStyleSheet(QString(R"(
        background: %1; color: white; border-radius: 10px;
        padding: 2px 12px; font-size: 12px; font-weight: bold;
    )").arg(badgeColor));
    catBadge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    catRow->addWidget(catLbl);
    catRow->addWidget(catBadge);
    catRow->addStretch();
    layout->addLayout(catRow);
}

void VendorDashboard::buildComplianceDocs(QVBoxLayout* layout) {
    layout->addWidget(makeSectionTitle("Compliance Documents"));
    layout->addWidget(makeDivider());

    QList<ComplianceDoc> docs = m_vendor->getComplianceDocs();
    for (const auto& doc : docs) {
        QFrame* docRow = new QFrame();
        docRow->setStyleSheet("QFrame { background: #F0F7F2; border: 1px solid #C8DDD0; border-radius: 5px; }");
        QVBoxLayout* dl = new QVBoxLayout(docRow);
        dl->setContentsMargins(12, 8, 12, 8);
        dl->setSpacing(3);

        QHBoxLayout* titleRow = new QHBoxLayout();
        QLabel* docType = new QLabel(doc.getTypeString());
        docType->setStyleSheet("font-weight: bold; font-size: 12px; color: #2B2016;");
        bool valid = doc.isValidForSeason();
        QLabel* status = new QLabel(valid ? "✓ Valid" : "⚠ Expiring");
        status->setStyleSheet(valid ?
            "color: #3D6B4F; font-size: 11px; font-weight: bold;" :
            "color: #C47C20; font-size: 11px; font-weight: bold;");
        titleRow->addWidget(docType);
        titleRow->addStretch();
        titleRow->addWidget(status);

        QLabel* docNum = new QLabel("Document #: " + doc.getDocNumber());
        docNum->setStyleSheet("font-size: 11px; color: #5C4A30;");
        QLabel* expiry = new QLabel("Expires: " + doc.getExpiry().toString("yyyy-MM-dd"));
        expiry->setStyleSheet("font-size: 11px; color: #5C4A30;");

        dl->addLayout(titleRow);
        dl->addWidget(docNum);
        dl->addWidget(expiry);
        if (!doc.getProvider().isEmpty()) {
            QLabel* provider = new QLabel("Provider: " + doc.getProvider());
            provider->setStyleSheet("font-size: 11px; color: #5C4A30;");
            dl->addWidget(provider);
        }
        layout->addWidget(docRow);
    }

    // Show missing docs — uses manager->getMissingDocs()
    for (const auto& m : m_manager->getMissingDocs(m_vendor)) {
        QFrame* missingRow = new QFrame();
        missingRow->setStyleSheet("QFrame { background: #FFF0F0; border: 1px solid #F0C0C0; border-radius: 5px; }");
        QHBoxLayout* ml = new QHBoxLayout(missingRow);
        ml->setContentsMargins(12, 8, 12, 8);
        QLabel* lbl = new QLabel("✗ MISSING: " + m);
        lbl->setStyleSheet("color: #B84040; font-size: 11px; font-weight: bold;");
        ml->addWidget(lbl);
        layout->addWidget(missingRow);
    }
}

void VendorDashboard::buildBookings(QVBoxLayout* layout) {
    layout->addWidget(makeSectionTitle("Active Stall Bookings"));
    layout->addWidget(makeDivider());

    // Uses manager->getBookingsForVendor() — not DataStore directly
    QList<Booking> bookings = m_manager->getBookingsForVendor(m_vendor);

    if (bookings.isEmpty()) {
        QLabel* empty = new QLabel("No active stall bookings. Visit the Market Schedule to book a stall.");
        empty->setStyleSheet("color: #7A5C3A; font-style: italic; font-size: 13px; padding: 8px 0;");
        layout->addWidget(empty);
        return;
    }

    QTableWidget* table = new QTableWidget(bookings.size(), 2);
    table->setHorizontalHeaderLabels({"Market Date", "Confirmation #"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setStyleSheet(StyleSheet::tableWidget() + "QTableWidget { alternate-background-color: #F5F0E8; }");

    for (int i = 0; i < bookings.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(bookings[i].getMarketDateString()));
        table->setItem(i, 1, new QTableWidgetItem(bookings[i].getConfirmationNumber()));
        table->setRowHeight(i, 36);
    }
    layout->addWidget(table);
}

void VendorDashboard::buildWaitlist(QVBoxLayout* layout) {
    layout->addWidget(makeSectionTitle("Waitlist Positions"));
    layout->addWidget(makeDivider());

    // Uses manager->getWaitlistForVendor() — not DataStore directly
    QList<WaitlistEntry> waitlist = m_manager->getWaitlistForVendor(m_vendor);

    if (waitlist.isEmpty()) {
        QLabel* empty = new QLabel("Not currently on any waitlist.");
        empty->setStyleSheet("color: #7A5C3A; font-style: italic; font-size: 13px; padding: 8px 0;");
        layout->addWidget(empty);
        return;
    }

    for (const auto& w : waitlist) {
        QFrame* row = new QFrame();
        row->setStyleSheet("QFrame { background: #FFF8E8; border: 1px solid #E8D4A0; border-radius: 5px; }");
        QHBoxLayout* rl = new QHBoxLayout(row);
        rl->setContentsMargins(12, 8, 12, 8);
        QLabel* dateLbl = new QLabel("📅  " + w.getMarketDateString());
        dateLbl->setStyleSheet("font-size: 13px; color: #2B2016;");
        QLabel* posLbl = new QLabel(QString("Queue Position: #%1").arg(w.getPosition()));
        posLbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #C47C20;");
        rl->addWidget(dateLbl);
        rl->addStretch();
        rl->addWidget(posLbl);
        layout->addWidget(row);
    }
}

void VendorDashboard::buildNotifications(QVBoxLayout* layout) {
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->addWidget(makeSectionTitle("System Notifications"));

    QStringList notifs = m_vendor->getNotifications();
    if (!notifs.isEmpty()) {
        QPushButton* clearBtn = new QPushButton("Clear All");
        clearBtn->setStyleSheet(R"(
            QPushButton { background: transparent; color: #7A5C3A; border: 1px solid #C4A882;
                          border-radius: 4px; padding: 3px 10px; font-size: 11px; }
            QPushButton:hover { background: #EDE7D9; }
        )");
        clearBtn->setCursor(Qt::PointingHandCursor);
        connect(clearBtn, &QPushButton::clicked, [this]() {
            m_vendor->clearNotifications();
            refresh();
        });
        headerRow->addStretch();
        headerRow->addWidget(clearBtn);
    }

    layout->addLayout(headerRow);
    layout->addWidget(makeDivider());

    if (notifs.isEmpty()) {
        QLabel* empty = new QLabel("No notifications.");
        empty->setStyleSheet("color: #7A5C3A; font-style: italic; font-size: 13px; padding: 8px 0;");
        layout->addWidget(empty);
        return;
    }

    for (const auto& notif : notifs.mid(0, 10)) {
        QString bg = notif.startsWith("✅") ? "#F0FFF4" :
                     notif.startsWith("❌") ? "#FFF5F5" :
                     notif.startsWith("🔔") ? "#FFF3CD" : "#F5F5FF";
        QString border = notif.startsWith("✅") ? "#B8E4C4" :
                         notif.startsWith("❌") ? "#F0C0C0" :
                         notif.startsWith("🔔") ? "#F0C040" : "#C0C8F0";
        QFrame* row = new QFrame();
        row->setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 5px; }").arg(bg).arg(border));
        QHBoxLayout* rl = new QHBoxLayout(row);
        rl->setContentsMargins(12, 8, 12, 8);
        QLabel* lbl = new QLabel(notif);
        lbl->setStyleSheet("font-size: 12px; color: #2B2016;");
        lbl->setWordWrap(true);
        rl->addWidget(lbl);
        layout->addWidget(row);
    }
}

void VendorDashboard::refresh() {
    QLayout* old = layout();
    if (old) {
        QLayoutItem* item;
        while ((item = old->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete old;
    }
    m_mainLayout = nullptr;
    setupUI();
}
