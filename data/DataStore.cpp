#include "DataStore.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

// ── Singleton ─────────────────────────────────────────────────────────────────

DataStore& DataStore::instance() {
    static DataStore store;
    return store;
}

// ── Initialize ────────────────────────────────────────────────────────────────

void DataStore::initialize() {
    if (!DatabaseManager::instance().open()) {
        qCritical() << "DataStore: failed to open database";
        return;
    }
    loadAll();
}

// ── Load all from SQLite into in-memory lists ─────────────────────────────────

void DataStore::loadAll() {
    m_vendors.clear();
    m_staff.clear();
    m_marketDates.clear();
    m_bookings.clear();
    m_waitlist.clear();

    loadUsers();
    loadMarketDates();
    loadBookings();
    loadWaitlist();
    loadNotifications();
}

void DataStore::loadUsers() {
    QSqlDatabase& db = DatabaseManager::instance().db();

    QSqlQuery q(db);
    q.exec(R"(
        SELECT u.id, u.username, u.display_name,
               v.business_name, v.owner_name, v.email, v.phone, v.address, v.category
        FROM users u
        JOIN vendors v ON v.id = u.id
        WHERE u.user_type = 0
        ORDER BY u.id
    )");

    while (q.next()) {
        int id = q.value(0).toInt();
        Vendor v(
            id,
            q.value(1).toString(),
            q.value(2).toString(),
            q.value(3).toString(),
            q.value(4).toString(),
            q.value(5).toString(),
            q.value(6).toString(),
            q.value(7).toString(),
            static_cast<VendorCategory>(q.value(8).toInt())
        );

        QSqlQuery dq(db);
        dq.prepare(R"(
            SELECT doc_type, doc_number, expiry_date, provider
            FROM compliance_docs
            WHERE vendor_id = ?
            ORDER BY id
        )");
        dq.addBindValue(id);
        dq.exec();
        while (dq.next()) {
            v.addComplianceDoc(ComplianceDoc(
                static_cast<DocType>(dq.value(0).toInt()),
                dq.value(1).toString(),
                QDate::fromString(dq.value(2).toString(), "yyyy-MM-dd"),
                dq.value(3).toString()
            ));
        }
        m_vendors.append(v);
    }

    QSqlQuery sq(db);
    sq.exec("SELECT id, username, display_name, user_type FROM users WHERE user_type IN (1,2) ORDER BY id");
    while (sq.next()) {
        m_staff.append(User(
            sq.value(0).toInt(),
            sq.value(1).toString(),
            sq.value(2).toString(),
            static_cast<UserType>(sq.value(3).toInt())
        ));
    }
}

void DataStore::loadMarketDates() {
    QSqlQuery q(DatabaseManager::instance().db());
    q.exec("SELECT id, date, max_food, max_artisan, booked_food, booked_artisan FROM market_dates ORDER BY date");
    while (q.next()) {
        MarketDate md(
            q.value(0).toInt(),
            QDate::fromString(q.value(1).toString(), "yyyy-MM-dd"),
            q.value(2).toInt(),
            q.value(3).toInt()
        );
        int bf = q.value(4).toInt();
        int ba = q.value(5).toInt();
        for (int i = 0; i < bf; ++i) md.bookFood();
        for (int i = 0; i < ba; ++i) md.bookArtisan();
        m_marketDates.append(md);
    }
}

void DataStore::loadBookings() {
    QSqlQuery q(DatabaseManager::instance().db());
    q.exec("SELECT id, vendor_id, market_date_id, market_date, confirmation_number FROM bookings ORDER BY id");
    while (q.next()) {
        m_bookings.append(Booking(
            q.value(0).toInt(),
            q.value(1).toInt(),
            q.value(2).toInt(),
            QDate::fromString(q.value(3).toString(), "yyyy-MM-dd"),
            q.value(4).toString()
        ));
    }
    QSqlQuery idq(DatabaseManager::instance().db());
    idq.exec("SELECT COALESCE(MAX(id), 0) + 1 FROM bookings");
    if (idq.next()) m_nextBookingId = idq.value(0).toInt();
}

void DataStore::loadWaitlist() {
    QSqlQuery q(DatabaseManager::instance().db());
    q.exec("SELECT id, vendor_id, market_date_id, market_date, category, position, notified FROM waitlist ORDER BY market_date_id, category, position");
    while (q.next()) {
        WaitlistEntry w(
            q.value(0).toInt(),
            q.value(1).toInt(),
            q.value(2).toInt(),
            QDate::fromString(q.value(3).toString(), "yyyy-MM-dd"),
            static_cast<VendorCategory>(q.value(4).toInt())
        );
        w.setPosition(q.value(5).toInt());
        w.setNotified(q.value(6).toBool());
        m_waitlist.append(w);
    }
    QSqlQuery idq(DatabaseManager::instance().db());
    idq.exec("SELECT COALESCE(MAX(id), 0) + 1 FROM waitlist");
    if (idq.next()) m_nextWaitlistId = idq.value(0).toInt();
}

void DataStore::loadNotifications() {
    QSqlQuery q(DatabaseManager::instance().db());
    q.exec("SELECT vendor_id, message FROM notifications ORDER BY vendor_id, id");
    while (q.next()) {
        Vendor* v = findVendorById(q.value(0).toInt());
        if (v) v->addNotification(q.value(1).toString());
    }
}

// ── User Lookup ───────────────────────────────────────────────────────────────

User* DataStore::findUser(const QString& username) {
    for (auto& v : m_vendors)
        if (v.getUsername().toLower() == username.toLower() ||
            v.getDisplayName().toLower() == username.toLower())
            return &v;
    for (auto& u : m_staff)
        if (u.getUsername().toLower() == username.toLower() ||
            u.getDisplayName().toLower() == username.toLower())
            return &u;
    return nullptr;
}

Vendor* DataStore::findVendor(const QString& username) {
    for (auto& v : m_vendors)
        if (v.getUsername().toLower() == username.toLower() ||
            v.getDisplayName().toLower() == username.toLower())
            return &v;
    return nullptr;
}

Vendor* DataStore::findVendorById(int id) {
    for (auto& v : m_vendors)
        if (v.getId() == id) return &v;
    return nullptr;
}

MarketDate* DataStore::findMarketDate(int id) {
    for (auto& md : m_marketDates)
        if (md.getId() == id) return &md;
    return nullptr;
}

// ── Bookings ──────────────────────────────────────────────────────────────────

QList<Booking> DataStore::getBookingsForVendor(int vendorId) const {
    QList<Booking> result;
    for (const auto& b : m_bookings)
        if (b.getVendorId() == vendorId) result.append(b);
    return result;
}

bool DataStore::vendorHasBooking(int vendorId, int marketDateId) const {
    for (const auto& b : m_bookings)
        if (b.getVendorId() == vendorId && b.getMarketDateId() == marketDateId)
            return true;
    return false;
}

bool DataStore::bookStall(int vendorId, int marketDateId) {
    Vendor*     vendor = findVendorById(vendorId);
    MarketDate* md     = findMarketDate(marketDateId);
    if (!vendor || !md) return false;
    if (vendorHasBooking(vendorId, marketDateId)) return false;

    bool isFood = (vendor->getCategory() == VendorCategory::Food);
    if (isFood  && !md->hasFoodAvailability())    return false;
    if (!isFood && !md->hasArtisanAvailability()) return false;

    QString confNum = QString("HM-%1-%2")
                      .arg(md->getDate().year())
                      .arg(m_nextBookingId, 4, 10, QChar('0'));
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlDatabase& db = DatabaseManager::instance().db();
    db.transaction();

    QSqlQuery q(db);
    q.prepare("INSERT INTO bookings (id, vendor_id, market_date_id, market_date, confirmation_number, created_at) VALUES (?,?,?,?,?,?)");
    q.addBindValue(m_nextBookingId);
    q.addBindValue(vendorId);
    q.addBindValue(marketDateId);
    q.addBindValue(md->getDate().toString("yyyy-MM-dd"));
    q.addBindValue(confNum);
    q.addBindValue(now);
    if (!q.exec()) { db.rollback(); return false; }

    QString col = isFood ? "booked_food" : "booked_artisan";
    q.prepare(QString("UPDATE market_dates SET %1 = %1 + 1 WHERE id = ?").arg(col));
    q.addBindValue(marketDateId);
    if (!q.exec()) { db.rollback(); return false; }

    writeAuditLog(db, vendorId, "BOOKING_CREATED",
        QString("vendor=%1 date=%2 conf=%3").arg(vendorId)
        .arg(md->getDate().toString("yyyy-MM-dd")).arg(confNum), now);

    db.commit();

    m_bookings.append(Booking(m_nextBookingId, vendorId, marketDateId, md->getDate(), confNum));
    m_nextBookingId++;
    if (isFood) md->bookFood(); else md->bookArtisan();

    QString msg = QString("✅ Booking confirmed for %1 (Conf: %2)")
                  .arg(md->getDate().toString("MMMM d, yyyy")).arg(confNum);
    persistNotification(db, vendorId, msg, now);
    vendor->addNotification(msg);

    return true;
}

bool DataStore::cancelBooking(int vendorId, int marketDateId) {
    Vendor*     vendor = findVendorById(vendorId);
    MarketDate* md     = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    int idx = -1;
    for (int i = 0; i < m_bookings.size(); ++i) {
        if (m_bookings[i].getVendorId() == vendorId &&
            m_bookings[i].getMarketDateId() == marketDateId) {
            idx = i; break;
        }
    }
    if (idx < 0) return false;

    bool isFood = (vendor->getCategory() == VendorCategory::Food);
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlDatabase& db = DatabaseManager::instance().db();
    db.transaction();

    QSqlQuery q(db);
    q.prepare("DELETE FROM bookings WHERE vendor_id = ? AND market_date_id = ?");
    q.addBindValue(vendorId);
    q.addBindValue(marketDateId);
    if (!q.exec()) { db.rollback(); return false; }

    QString col = isFood ? "booked_food" : "booked_artisan";
    q.prepare(QString("UPDATE market_dates SET %1 = MAX(0, %1 - 1) WHERE id = ?").arg(col));
    q.addBindValue(marketDateId);
    if (!q.exec()) { db.rollback(); return false; }

    writeAuditLog(db, vendorId, "BOOKING_CANCELLED",
        QString("vendor=%1 date=%2").arg(vendorId)
        .arg(md->getDate().toString("yyyy-MM-dd")), now);

    db.commit();

    m_bookings.removeAt(idx);
    if (isFood) md->cancelFood(); else md->cancelArtisan();

    QString msg = QString("❌ Booking cancelled for %1")
                  .arg(md->getDate().toString("MMMM d, yyyy"));
    persistNotification(db, vendorId, msg, now);
    vendor->addNotification(msg);

    processWaitlistOnCancellation(marketDateId, vendor->getCategory());
    return true;
}

// ── Waitlist ──────────────────────────────────────────────────────────────────

QList<WaitlistEntry> DataStore::getWaitlistForVendor(int vendorId) const {
    QList<WaitlistEntry> result;
    for (const auto& w : m_waitlist)
        if (w.getVendorId() == vendorId) result.append(w);
    return result;
}

bool DataStore::vendorOnWaitlist(int vendorId, int marketDateId) const {
    for (const auto& w : m_waitlist)
        if (w.getVendorId() == vendorId && w.getMarketDateId() == marketDateId)
            return true;
    return false;
}

int DataStore::getWaitlistPosition(int vendorId, int marketDateId) const {
    Vendor* vendor = const_cast<DataStore*>(this)->findVendorById(vendorId);
    if (!vendor) return -1;
    int pos = 1;
    for (const auto& w : m_waitlist) {
        if (w.getMarketDateId() == marketDateId && w.getCategory() == vendor->getCategory()) {
            if (w.getVendorId() == vendorId) return pos;
            ++pos;
        }
    }
    return -1;
}

bool DataStore::joinWaitlist(int vendorId, int marketDateId) {
    if (vendorOnWaitlist(vendorId, marketDateId)) return false;
    Vendor*     vendor = findVendorById(vendorId);
    MarketDate* md     = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    int queueLen = 0;
    for (const auto& w : m_waitlist)
        if (w.getMarketDateId() == marketDateId && w.getCategory() == vendor->getCategory())
            ++queueLen;
    int newPos = queueLen + 1;
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlDatabase& db = DatabaseManager::instance().db();
    db.transaction();

    QSqlQuery q(db);
    q.prepare("INSERT INTO waitlist (id, vendor_id, market_date_id, market_date, category, position, notified, joined_at) VALUES (?,?,?,?,?,?,0,?)");
    q.addBindValue(m_nextWaitlistId);
    q.addBindValue(vendorId);
    q.addBindValue(marketDateId);
    q.addBindValue(md->getDate().toString("yyyy-MM-dd"));
    q.addBindValue(static_cast<int>(vendor->getCategory()));
    q.addBindValue(newPos);
    q.addBindValue(now);
    if (!q.exec()) { db.rollback(); return false; }

    writeAuditLog(db, vendorId, "WAITLIST_JOINED",
        QString("vendor=%1 date=%2 pos=%3").arg(vendorId)
        .arg(md->getDate().toString("yyyy-MM-dd")).arg(newPos), now);

    db.commit();

    WaitlistEntry w(m_nextWaitlistId, vendorId, marketDateId, md->getDate(), vendor->getCategory());
    w.setPosition(newPos);
    m_waitlist.append(w);
    m_nextWaitlistId++;

    QString msg = QString("⏳ Added to waitlist for %1 (Position: %2)")
                  .arg(md->getDate().toString("MMMM d, yyyy")).arg(newPos);
    persistNotification(db, vendorId, msg, now);
    vendor->addNotification(msg);

    return true;
}

bool DataStore::leaveWaitlist(int vendorId, int marketDateId) {
    Vendor*     vendor = findVendorById(vendorId);
    MarketDate* md     = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    int idx = -1;
    for (int i = 0; i < m_waitlist.size(); ++i) {
        if (m_waitlist[i].getVendorId() == vendorId &&
            m_waitlist[i].getMarketDateId() == marketDateId) {
            idx = i; break;
        }
    }
    if (idx < 0) return false;

    int removedPos = m_waitlist[idx].getPosition();
    VendorCategory cat = vendor->getCategory();
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlDatabase& db = DatabaseManager::instance().db();
    db.transaction();

    QSqlQuery q(db);
    q.prepare("DELETE FROM waitlist WHERE vendor_id = ? AND market_date_id = ?");
    q.addBindValue(vendorId);
    q.addBindValue(marketDateId);
    if (!q.exec()) { db.rollback(); return false; }

    q.prepare("UPDATE waitlist SET position = position - 1 WHERE market_date_id = ? AND category = ? AND position > ?");
    q.addBindValue(marketDateId);
    q.addBindValue(static_cast<int>(cat));
    q.addBindValue(removedPos);
    q.exec();

    writeAuditLog(db, vendorId, "WAITLIST_LEFT",
        QString("vendor=%1 date=%2").arg(vendorId)
        .arg(md->getDate().toString("yyyy-MM-dd")), now);

    db.commit();

    m_waitlist.removeAt(idx);
    int pos = 1;
    for (auto& w : m_waitlist)
        if (w.getMarketDateId() == marketDateId && w.getCategory() == cat)
            w.setPosition(pos++);

    QString msg = QString("🚫 Removed from waitlist for %1")
                  .arg(md->getDate().toString("MMMM d, yyyy"));
    persistNotification(db, vendorId, msg, now);
    vendor->addNotification(msg);

    return true;
}

void DataStore::processWaitlistOnCancellation(int marketDateId, VendorCategory category) {
    for (auto& w : m_waitlist) {
        if (w.getMarketDateId() == marketDateId &&
            w.getCategory() == category &&
            w.getPosition() == 1) {

            Vendor*     vendor = findVendorById(w.getVendorId());
            MarketDate* md     = findMarketDate(marketDateId);
            if (!vendor || !md) return;

            w.setNotified(true);

            QSqlDatabase& db = DatabaseManager::instance().db();
            QSqlQuery q(db);
            q.prepare("UPDATE waitlist SET notified = 1 WHERE vendor_id = ? AND market_date_id = ?");
            q.addBindValue(w.getVendorId());
            q.addBindValue(marketDateId);
            q.exec();

            QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            QString msg = QString("🔔 A stall is now available for %1! You are #1 on the waitlist. Please book your stall.")
                          .arg(md->getDate().toString("MMMM d, yyyy"));

            persistNotification(db, w.getVendorId(), msg, now);
            vendor->addNotification(msg);
            return;
        }
    }
}

// ── Private helpers ───────────────────────────────────────────────────────────

void DataStore::persistNotification(QSqlDatabase& db, int vendorId,
                                     const QString& message, const QString& now) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO notifications (vendor_id, message, created_at) VALUES (?,?,?)");
    q.addBindValue(vendorId);
    q.addBindValue(message);
    q.addBindValue(now);
    q.exec();
}

void DataStore::writeAuditLog(QSqlDatabase& db, int userId, const QString& action,
                               const QString& details, const QString& now) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO audit_log (user_id, action, details, created_at) VALUES (?,?,?,?)");
    q.addBindValue(userId);
    q.addBindValue(action);
    q.addBindValue(details);
    q.addBindValue(now);
    q.exec();
}
