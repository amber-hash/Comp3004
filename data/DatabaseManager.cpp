#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDate>

// ── Singleton ─────────────────────────────────────────────────────────────────

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager mgr;
    return mgr;
}

// ── Open / Close ──────────────────────────────────────────────────────────────

bool DatabaseManager::open(const QString& path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qCritical() << "DatabaseManager: failed to open" << path
                    << m_db.lastError().text();
        return false;
    }

    // Enable WAL mode and foreign keys
    execQuery("PRAGMA journal_mode=WAL");
    execQuery("PRAGMA foreign_keys=ON");

    if (!createTables()) return false;
    if (!seedIfEmpty())  return false;

    qDebug() << "DatabaseManager: opened" << path;
    return true;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

// ── Schema ────────────────────────────────────────────────────────────────────

bool DatabaseManager::createTables() {

    // ── users ────────────────────────────────────────────────────────────────
    // Stores all user types: Vendor=0, MarketOperator=1, SystemAdmin=2
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS users (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            username     TEXT    NOT NULL UNIQUE,
            display_name TEXT    NOT NULL,
            user_type    INTEGER NOT NULL   -- 0=Vendor, 1=MarketOperator, 2=SystemAdmin
        )
    )")) return false;

    // ── vendors ───────────────────────────────────────────────────────────────
    // One row per vendor. References users(id).
    // category: 0=Food, 1=Artisan
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS vendors (
            id            INTEGER PRIMARY KEY,
            business_name TEXT NOT NULL,
            owner_name    TEXT NOT NULL,
            email         TEXT NOT NULL,
            phone         TEXT NOT NULL,
            address       TEXT NOT NULL,
            category      INTEGER NOT NULL,  -- 0=Food, 1=Artisan
            FOREIGN KEY (id) REFERENCES users(id)
        )
    )")) return false;

    // ── compliance_docs ───────────────────────────────────────────────────────
    // doc_type: 0=BusinessLicence, 1=LiabilityInsurance, 2=FoodHandlerCert
    // provider is only used for LiabilityInsurance
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS compliance_docs (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            vendor_id   INTEGER NOT NULL,
            doc_type    INTEGER NOT NULL,  -- 0=BusinessLicence, 1=LiabilityInsurance, 2=FoodHandlerCert
            doc_number  TEXT    NOT NULL,
            expiry_date TEXT    NOT NULL,  -- ISO 8601: YYYY-MM-DD
            provider    TEXT    DEFAULT '',
            FOREIGN KEY (vendor_id) REFERENCES vendors(id)
        )
    )")) return false;

    // ── market_dates ──────────────────────────────────────────────────────────
    // Each row is one Sunday market day in the season.
    // max_food/max_artisan default to 2 for D1 (20/10 in production)
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS market_dates (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            date         TEXT    NOT NULL UNIQUE,  -- ISO 8601: YYYY-MM-DD
            max_food     INTEGER NOT NULL DEFAULT 2,
            max_artisan  INTEGER NOT NULL DEFAULT 2,
            booked_food  INTEGER NOT NULL DEFAULT 0,
            booked_artisan INTEGER NOT NULL DEFAULT 0
        )
    )")) return false;

    // ── bookings ──────────────────────────────────────────────────────────────
    // One row per confirmed stall booking.
    // confirmation_number: human-readable, e.g. "HM-2025-0001"
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS bookings (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            vendor_id           INTEGER NOT NULL,
            market_date_id      INTEGER NOT NULL,
            market_date         TEXT    NOT NULL,  -- ISO 8601, denormalized for fast reads
            confirmation_number TEXT    NOT NULL,
            created_at          TEXT    NOT NULL,  -- ISO 8601 timestamp
            UNIQUE (vendor_id, market_date_id),    -- no double booking
            FOREIGN KEY (vendor_id)      REFERENCES vendors(id),
            FOREIGN KEY (market_date_id) REFERENCES market_dates(id)
        )
    )")) return false;

    // ── waitlist ──────────────────────────────────────────────────────────────
    // FIFO queue per category per market date.
    // position is 1-based; recalculated on any removal.
    // notified=1 means the vendor has been told a stall opened.
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS waitlist (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            vendor_id      INTEGER NOT NULL,
            market_date_id INTEGER NOT NULL,
            market_date    TEXT    NOT NULL,  -- ISO 8601
            category       INTEGER NOT NULL, -- 0=Food, 1=Artisan
            position       INTEGER NOT NULL,
            notified       INTEGER NOT NULL DEFAULT 0,  -- 0=false, 1=true
            joined_at      TEXT    NOT NULL,  -- ISO 8601 timestamp, used for FIFO ordering
            UNIQUE (vendor_id, market_date_id),
            FOREIGN KEY (vendor_id)      REFERENCES vendors(id),
            FOREIGN KEY (market_date_id) REFERENCES market_dates(id)
        )
    )")) return false;

    // ── notifications ─────────────────────────────────────────────────────────
    // Per-vendor notification inbox. Displayed on the dashboard.
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS notifications (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            vendor_id  INTEGER NOT NULL,
            message    TEXT    NOT NULL,
            created_at TEXT    NOT NULL,  -- ISO 8601 timestamp
            FOREIGN KEY (vendor_id) REFERENCES vendors(id)
        )
    )")) return false;

    // ── audit_log ─────────────────────────────────────────────────────────────
    // Append-only log of every state-changing action (§2.13).
    // action examples: "BOOKING_CREATED", "BOOKING_CANCELLED", "WAITLIST_JOINED", etc.
    if (!execQuery(R"(
        CREATE TABLE IF NOT EXISTS audit_log (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id    INTEGER NOT NULL,
            action     TEXT    NOT NULL,
            details    TEXT    NOT NULL,
            created_at TEXT    NOT NULL   -- ISO 8601 timestamp
        )
    )")) return false;

    qDebug() << "DatabaseManager: all tables ready";
    return true;
}

// ── Seed ──────────────────────────────────────────────────────────────────────

bool DatabaseManager::seedIfEmpty() {
    QSqlQuery q(m_db);
    q.exec("SELECT COUNT(*) FROM users");
    if (q.next() && q.value(0).toInt() > 0) {
        qDebug() << "DatabaseManager: database already seeded, skipping";
        return true;
    }
    qDebug() << "DatabaseManager: seeding initial data...";
    return seedUsers() && seedMarketDates();
}

bool DatabaseManager::seedUsers() {
    // Helper lambda: insert into users table, return new id
    auto insertUser = [&](const QString& username, const QString& displayName, int userType) -> int {
        QSqlQuery q(m_db);
        q.prepare("INSERT INTO users (username, display_name, user_type) VALUES (?, ?, ?)");
        q.addBindValue(username);
        q.addBindValue(displayName);
        q.addBindValue(userType);
        if (!q.exec()) {
            qWarning() << "seedUsers: insert user failed:" << q.lastError().text();
            return -1;
        }
        return q.lastInsertId().toInt();
    };

    // Helper lambda: insert into vendors table
    auto insertVendor = [&](int id, const QString& biz, const QString& owner,
                             const QString& email, const QString& phone,
                             const QString& addr, int category) -> bool {
        QSqlQuery q(m_db);
        q.prepare(R"(INSERT INTO vendors
                     (id, business_name, owner_name, email, phone, address, category)
                     VALUES (?, ?, ?, ?, ?, ?, ?))");
        q.addBindValue(id);
        q.addBindValue(biz);
        q.addBindValue(owner);
        q.addBindValue(email);
        q.addBindValue(phone);
        q.addBindValue(addr);
        q.addBindValue(category);
        if (!q.exec()) {
            qWarning() << "seedUsers: insert vendor failed:" << q.lastError().text();
            return false;
        }
        return true;
    };

    // Helper lambda: insert compliance doc
    auto insertDoc = [&](int vendorId, int docType, const QString& docNum,
                          const QString& expiry, const QString& provider = "") -> bool {
        QSqlQuery q(m_db);
        q.prepare(R"(INSERT INTO compliance_docs
                     (vendor_id, doc_type, doc_number, expiry_date, provider)
                     VALUES (?, ?, ?, ?, ?))");
        q.addBindValue(vendorId);
        q.addBindValue(docType);
        q.addBindValue(docNum);
        q.addBindValue(expiry);
        q.addBindValue(provider);
        if (!q.exec()) {
            qWarning() << "seedUsers: insert doc failed:" << q.lastError().text();
            return false;
        }
        return true;
    };

    // ── 12 Food Vendors ───────────────────────────────────────────────────────
    // doc_type: 0=BusinessLicence, 1=LiabilityInsurance, 2=FoodHandlerCert

    // 1. Fresh Harvest Farm — fully compliant
    { int id = insertUser("freshharvest", "Fresh Harvest Farm", 0);
      insertVendor(id, "Fresh Harvest Farm", "Alice Johnson",
                   "alice@freshharvest.com", "613-555-0101",
                   "123 Farm Road, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0045",  "2026-12-31");
      insertDoc(id, 1, "POL-8821-FH",   "2026-12-31", "Intact Insurance");
      insertDoc(id, 2, "OFH-2024-9912", "2026-08-15"); }

    // 2. Sunrise Bakery — fully compliant
    { int id = insertUser("sunrisebakery", "Sunrise Bakery", 0);
      insertVendor(id, "Sunrise Bakery", "Bob Martin",
                   "bob@sunrisebakery.com", "613-555-0202",
                   "45 Baker Street, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0088",  "2026-11-30");
      insertDoc(id, 1, "POL-5544-SB",   "2026-11-30", "Desjardins Insurance");
      insertDoc(id, 2, "OFH-2024-3301", "2026-10-01"); }

    // 3. Green Valley Organics — fully compliant
    { int id = insertUser("greenvalley", "Green Valley Organics", 0);
      insertVendor(id, "Green Valley Organics", "Carol White",
                   "carol@greenvalley.com", "613-555-0303",
                   "78 Organic Lane, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0112",  "2026-10-31");
      insertDoc(id, 1, "POL-7723-GV",   "2026-10-31", "Aviva Canada");
      insertDoc(id, 2, "OFH-2025-1104", "2026-09-30"); }

    // 4. Maple Ridge Preserves — MISSING food handler cert (tests compliance blocking)
    { int id = insertUser("maplesyrup", "Maple Ridge Preserves", 0);
      insertVendor(id, "Maple Ridge Preserves", "David Lee",
                   "david@mapleridge.com", "613-555-0404",
                   "9 Maple Drive, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0156", "2026-12-31");
      insertDoc(id, 1, "POL-3310-MR",  "2026-12-31", "TD Insurance"); }

    // 5. Riverbend Honey — fully compliant
    { int id = insertUser("riverbendhoney", "Riverbend Honey Co.", 0);
      insertVendor(id, "Riverbend Honey Co.", "Sarah Thompson",
                   "sarah@riverbendhoney.com", "613-555-0505",
                   "200 River Road, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0178",  "2026-12-31");
      insertDoc(id, 1, "POL-2200-RH",   "2026-12-31", "Manulife");
      insertDoc(id, 2, "OFH-2025-4421", "2026-12-31"); }

    // 6. Hintonville Hot Sauce — fully compliant
    { int id = insertUser("hotsauce", "Hintonville Hot Sauce", 0);
      insertVendor(id, "Hintonville Hot Sauce", "Mike Ruiz",
                   "mike@hintonvillehotsauce.com", "613-555-0606",
                   "17 Pepper Ave, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0191",  "2026-12-31");
      insertDoc(id, 1, "POL-4400-HS",   "2026-12-31", "Co-operators");
      insertDoc(id, 2, "OFH-2025-5512", "2026-12-31"); }

    // 7. The Soup Kettle — EXPIRING food handler cert (expires before season ends)
    { int id = insertUser("soupkettle", "The Soup Kettle", 0);
      insertVendor(id, "The Soup Kettle", "Linda Park",
                   "linda@soupkettle.com", "613-555-0707",
                   "55 Broth Lane, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0204",  "2026-12-31");
      insertDoc(id, 1, "POL-6610-SK",   "2026-12-31", "Sun Life");
      insertDoc(id, 2, "OFH-2023-0099", "2025-07-01"); } // expired

    // 8. Cedar Creek Cider — fully compliant
    { int id = insertUser("cedarcreek", "Cedar Creek Cider", 0);
      insertVendor(id, "Cedar Creek Cider", "James Olson",
                   "james@cedarcreekcider.com", "613-555-0808",
                   "33 Orchard Way, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0217",  "2026-12-31");
      insertDoc(id, 1, "POL-8830-CC",   "2026-12-31", "Economical Insurance");
      insertDoc(id, 2, "OFH-2025-6633", "2026-12-31"); }

    // 9. Morning Glory Mushrooms — fully compliant
    { int id = insertUser("morningglory", "Morning Glory Mushrooms", 0);
      insertVendor(id, "Morning Glory Mushrooms", "Fiona Chen",
                   "fiona@morningglory.com", "613-555-0909",
                   "88 Forest Path, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0229",  "2026-12-31");
      insertDoc(id, 1, "POL-9940-MG",   "2026-12-31", "Intact Insurance");
      insertDoc(id, 2, "OFH-2025-7744", "2026-12-31"); }

    // 10. Prairie Wind Bakehouse — fully compliant
    { int id = insertUser("prairiewind", "Prairie Wind Bakehouse", 0);
      insertVendor(id, "Prairie Wind Bakehouse", "Tom Bergmann",
                   "tom@prairiewind.com", "613-555-1010",
                   "14 Wheat Street, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0241",  "2026-12-31");
      insertDoc(id, 1, "POL-1150-PW",   "2026-12-31", "Desjardins Insurance");
      insertDoc(id, 2, "OFH-2025-8855", "2026-12-31"); }

    // 11. Harvest Moon Jams — fully compliant
    { int id = insertUser("harvestmoon", "Harvest Moon Jams", 0);
      insertVendor(id, "Harvest Moon Jams", "Grace Nguyen",
                   "grace@harvestmoonjams.com", "613-555-1111",
                   "66 Orchard Blvd, Hintonville, ON", 0);
      insertDoc(id, 0, "BL-2025-0253",  "2026-12-31");
      insertDoc(id, 1, "POL-2260-HM",   "2026-12-31", "Aviva Canada");
      insertDoc(id, 2, "OFH-2025-9966", "2026-12-31"); }

    // 12. Two Rivers Sprouts — no documents at all (tests full compliance block)
    { int id = insertUser("tworiverssprouts", "Two Rivers Sprouts", 0);
      insertVendor(id, "Two Rivers Sprouts", "Omar Hassan",
                   "omar@tworiverssprouts.com", "613-555-1212",
                   "101 Sprout Row, Hintonville, ON", 0); }

    // ── 8 Artisan Vendors ─────────────────────────────────────────────────────
    // doc_type: 0=BusinessLicence, 1=LiabilityInsurance (no food cert required)

    // 13. Clay Creations Studio — fully compliant
    { int id = insertUser("claycreations", "Clay Creations Studio", 0);
      insertVendor(id, "Clay Creations Studio", "Emma Brown",
                   "emma@claycreations.com", "613-555-1313",
                   "22 Potter Street, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0201", "2026-12-31");
      insertDoc(id, 1, "POL-9900-CC",  "2026-12-31", "Sun Life"); }

    // 14. WoodCraft Workshop — fully compliant
    { int id = insertUser("woodcraft", "WoodCraft Workshop", 0);
      insertVendor(id, "WoodCraft Workshop", "Frank Davis",
                   "frank@woodcraft.com", "613-555-1414",
                   "55 Timber Road, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0233", "2026-12-31");
      insertDoc(id, 1, "POL-1122-WC",  "2026-12-31", "Manulife"); }

    // 15. Silk & Thread Textiles — fully compliant
    { int id = insertUser("silkthread", "Silk & Thread Textiles", 0);
      insertVendor(id, "Silk & Thread Textiles", "Grace Kim",
                   "grace@silkthread.com", "613-555-1515",
                   "33 Weaver Ave, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0277", "2026-12-31");
      insertDoc(id, 1, "POL-4455-ST",  "2026-12-31", "Co-operators"); }

    // 16. The Jewelry Box — fully compliant
    { int id = insertUser("jewelrybox", "The Jewelry Box", 0);
      insertVendor(id, "The Jewelry Box", "Hannah Park",
                   "hannah@thejewelrybox.com", "613-555-1616",
                   "12 Gem Court, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0299", "2026-12-31");
      insertDoc(id, 1, "POL-6677-JB",  "2026-12-31", "Economical Insurance"); }

    // 17. Northern Lights Candles — fully compliant
    { int id = insertUser("northernlights", "Northern Lights Candles", 0);
      insertVendor(id, "Northern Lights Candles", "Ivan Petrov",
                   "ivan@northernlightscandles.com", "613-555-1717",
                   "77 Wick Way, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0311", "2026-12-31");
      insertDoc(id, 1, "POL-7788-NL",  "2026-12-31", "TD Insurance"); }

    // 18. Ironwood Forge — fully compliant
    { int id = insertUser("ironwoodforge", "Ironwood Forge", 0);
      insertVendor(id, "Ironwood Forge", "Rachel Stone",
                   "rachel@ironwoodforge.com", "613-555-1818",
                   "44 Anvil Road, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0322", "2026-12-31");
      insertDoc(id, 1, "POL-8899-IF",  "2026-12-31", "Intact Insurance"); }

    // 19. Mosaic & More — MISSING insurance (tests compliance blocking for artisan)
    { int id = insertUser("mosaicandmore", "Mosaic & More", 0);
      insertVendor(id, "Mosaic & More", "Priya Sharma",
                   "priya@mosaicandmore.com", "613-555-1919",
                   "99 Tile Lane, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0334", "2026-12-31"); } // no insurance

    // 20. Birchbark Studio — fully compliant
    { int id = insertUser("birchbark", "Birchbark Studio", 0);
      insertVendor(id, "Birchbark Studio", "Liam Tremblay",
                   "liam@birchbarkstudio.com", "613-555-2020",
                   "11 Birch Ave, Hintonville, ON", 1);
      insertDoc(id, 0, "BL-2025-0345", "2026-12-31");
      insertDoc(id, 1, "POL-9911-BB",  "2026-12-31", "Desjardins Insurance"); }

    // ── Staff ─────────────────────────────────────────────────────────────────
    insertUser("operator", "Market Operator",      1);  // MarketOperator
    insertUser("admin",    "System Administrator", 2);  // SystemAdmin

    qDebug() << "DatabaseManager: seeded 20 vendors + 2 staff";
    return true;
}

bool DatabaseManager::seedMarketDates() {
    // Seed 10 upcoming Sundays from next Sunday onward.
    // In a real season these would be May–September Sundays.
    // For D1 we generate them dynamically from today.
    QDate today = QDate::currentDate();
    // Find the next Sunday
    int daysUntilSunday = (7 - today.dayOfWeek()) % 7;
    if (daysUntilSunday == 0) daysUntilSunday = 7;
    QDate nextSunday = today.addDays(daysUntilSunday);

    for (int i = 0; i < 10; ++i) {
        QDate d = nextSunday.addDays(i * 7);
        QSqlQuery q(m_db);
        q.prepare(R"(INSERT OR IGNORE INTO market_dates
                     (date, max_food, max_artisan, booked_food, booked_artisan)
                     VALUES (?, 2, 2, 0, 0))");
        q.addBindValue(d.toString("yyyy-MM-dd"));
        if (!q.exec()) {
            qWarning() << "seedMarketDates: failed:" << q.lastError().text();
            return false;
        }
    }

    qDebug() << "DatabaseManager: seeded 10 market dates";
    return true;
}

// ── Utility ───────────────────────────────────────────────────────────────────

bool DatabaseManager::execQuery(const QString& sql) {
    QSqlQuery q(m_db);
    if (!q.exec(sql)) {
        qWarning() << "DatabaseManager::execQuery failed:" << q.lastError().text()
                   << "\nSQL:" << sql;
        return false;
    }
    return true;
}
