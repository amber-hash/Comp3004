#include "DataStore.h"
#include <QDate>

DataStore& DataStore::instance() {
    static DataStore store;
    return store;
}

void DataStore::initialize() {
    m_vendors.clear();
    m_staff.clear();
    m_marketDates.clear();
    m_bookings.clear();
    m_waitlist.clear();
    m_nextBookingId = 1;
    m_nextWaitlistId = 1;
    seedUsers();
    seedMarketDates();
}

void DataStore::seedUsers() {
    // 5 food vendors
    {
        Vendor v(1, "freshharvest", "Fresh Harvest Farm", "Fresh Harvest Farm",
                 "Alice Johnson", "alice@freshharvest.com", "613-555-0101",
                 "123 Farm Road, Hintonville, ON", VendorCategory::Food);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-202-0045",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-8821-FH",
                                          QDate(2027, 12, 31), "Intact Insurance"));
        v.addComplianceDoc(ComplianceDoc(DocType::FoodHandlerCert, "OFH-2024-9912",
                                          QDate(2027, 8, 15)));
        m_vendors.append(v);
    }
    {
        Vendor v(2, "sunrisebakery", "Sunrise Bakery", "Sunrise Bakery",
                 "Bob Martin", "bob@sunrisebakery.com", "613-555-0202",
                 "45 Baker Street, Hintonville, ON", VendorCategory::Food);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0088",
                                          QDate(2027, 11, 30)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-5544-SB",
                                          QDate(2027, 11, 30), "Desjardins Insurance"));
        v.addComplianceDoc(ComplianceDoc(DocType::FoodHandlerCert, "OFH-2024-3301",
                                          QDate(2027, 10, 1)));
        m_vendors.append(v);
    }
    {
        Vendor v(3, "greenvalley", "Green Valley Organics", "Green Valley Organics",
                 "Carol White", "carol@greenvalley.com", "613-555-0303",
                 "78 Organic Lane, Hintonville, ON", VendorCategory::Food);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0112",
                                          QDate(2026, 10, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-7723-GV",
                                          QDate(2026, 10, 31), "Aviva Canada"));
        v.addComplianceDoc(ComplianceDoc(DocType::FoodHandlerCert, "OFH-2025-1104",
                                          QDate(2027, 9, 30)));
        m_vendors.append(v);
    }
    {
        Vendor v(4, "maplesyrup", "Maple Ridge Preserves", "Maple Ridge Preserves",
                 "David Lee", "david@mapleridge.com", "613-555-0404",
                 "9 Maple Drive, Hintonville, ON", VendorCategory::Food);
        // Missing food handler cert - to test compliance blocking
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0156",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-3310-MR",
                                          QDate(2027, 12, 31), "TD Insurance"));
        m_vendors.append(v);
    }

    {
        Vendor v(11, "riversidefarm", "Riverside Farm", "Riverside Farm",
                 "Tom Harris", "tom@riversidefarm.com", "613-555-0909",
                 "99 River Road, Hintonville, ON", VendorCategory::Food);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0400",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-1234-RF",
                                          QDate(2027, 12, 31), "Intact Insurance"));
        v.addComplianceDoc(ComplianceDoc(DocType::FoodHandlerCert, "OFH-2025-5500",
                                          QDate(2027, 8, 15)));
        m_vendors.append(v);
    }

    // 4 artisan vendors
    {
        Vendor v(5, "claycreations", "Clay Creations Studio", "Clay Creations Studio",
                 "Emma Brown", "emma@claycreations.com", "613-555-0505",
                 "22 Potter Street, Hintonville, ON", VendorCategory::Artisan);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0201",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-9900-CC",
                                          QDate(2027, 12, 31), "Sun Life"));
        m_vendors.append(v);
    }
    {
        Vendor v(6, "woodcraft", "WoodCraft Workshop", "WoodCraft Workshop",
                 "Frank Davis", "frank@woodcraft.com", "613-555-0606",
                 "55 Timber Road, Hintonville, ON", VendorCategory::Artisan);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0233",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-1122-WC",
                                          QDate(2027, 12, 31), "Manulife"));
        m_vendors.append(v);
    }
    {
        Vendor v(7, "silkthread", "Silk & Thread Textiles", "Silk & Thread Textiles",
                 "Grace Kim", "grace@silkthread.com", "613-555-0707",
                 "33 Weaver Ave, Hintonville, ON", VendorCategory::Artisan);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0277",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-4455-ST",
                                          QDate(2027, 12, 31), "Co-operators"));
        m_vendors.append(v);
    }
    {
        Vendor v(8, "jewelrybox", "The Jewelry Box", "The Jewelry Box",
                 "Hannah Park", "hannah@thejewelrybox.com", "613-555-0808",
                 "12 Gem Court, Hintonville, ON", VendorCategory::Artisan);
        v.addComplianceDoc(ComplianceDoc(DocType::BusinessLicence, "BL-2025-0299",
                                          QDate(2027, 12, 31)));
        v.addComplianceDoc(ComplianceDoc(DocType::LiabilityInsurance, "POL-6677-JB",
                                          QDate(2027, 12, 31), "Economical Insurance"));
        m_vendors.append(v);
    }
    // Market operator
    m_staff.append(User(9, "operator", "Market Operator", UserType::MarketOperator));
    // System admin
    m_staff.append(User(10, "admin", "System Administrator", UserType::SystemAdmin));
}

void DataStore::seedMarketDates() {
    // 4 upcoming Sundays starting from next Sunday
    QDate today = QDate::currentDate();
    QDate nextSunday = today.addDays((7 - today.dayOfWeek()) % 7);
    if (nextSunday == today) nextSunday = today.addDays(7);

    for (int i = 0; i < 4; ++i) {
        QDate d = nextSunday.addDays(i * 7);
        m_marketDates.append(MarketDate(i + 1, d, 2, 2));
    }
}

User* DataStore::findUser(const QString& username) {
    for (auto& v : m_vendors) {
        if (v.getUsername().toLower() == username.toLower() ||
            v.getDisplayName().toLower() == username.toLower()) {
            return &v;
        }
    }
    for (auto& u : m_staff) {
        if (u.getUsername().toLower() == username.toLower() ||
            u.getDisplayName().toLower() == username.toLower()) {
            return &u;
        }
    }
    return nullptr;
}

Vendor* DataStore::findVendor(const QString& username) {
    for (auto& v : m_vendors) {
        if (v.getUsername().toLower() == username.toLower() ||
            v.getDisplayName().toLower() == username.toLower()) {
            return &v;
        }
    }
    return nullptr;
}

Vendor* DataStore::findVendorById(int id) {
    for (auto& v : m_vendors) {
        if (v.getId() == id) return &v;
    }
    return nullptr;
}

MarketDate* DataStore::findMarketDate(int id) {
    for (auto& md : m_marketDates) {
        if (md.getId() == id) return &md;
    }
    return nullptr;
}

QList<Booking> DataStore::getBookingsForVendor(int vendorId) const {
    QList<Booking> result;
    for (const auto& b : m_bookings) {
        if (b.getVendorId() == vendorId) result.append(b);
    }
    return result;
}

bool DataStore::vendorHasBooking(int vendorId, int marketDateId) const {
    for (const auto& b : m_bookings) {
        if (b.getVendorId() == vendorId && b.getMarketDateId() == marketDateId)
            return true;
    }
    return false;
}

bool DataStore::bookStall(int vendorId, int marketDateId) {
    Vendor* vendor = findVendorById(vendorId);
    MarketDate* md = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    // Compliance check — vendor must have all required documents valid for the season
    if (!vendor->hasAllRequiredDocs()) return false;

    // One active booking at a time — spec: "Vendors can book only one market stall date at a time"
    if (!getBookingsForVendor(vendorId).isEmpty()) return false;

    // No duplicate booking for the same date
    if (vendorHasBooking(vendorId, marketDateId)) return false;

    bool available = (vendor->getCategory() == VendorCategory::Food)
                     ? md->hasFoodAvailability()
                     : md->hasArtisanAvailability();
    if (!available) return false;

    Booking b(m_nextBookingId++, vendorId, marketDateId, md->getDate());
    m_bookings.append(b);

    if (vendor->getCategory() == VendorCategory::Food) md->bookFood();
    else md->bookArtisan();

    leaveWaitlist(vendorId, marketDateId); //removes vendor fromm waitlist

    vendor->addNotification(QString("✅ Booking confirmed for %1 (Conf: %2)")
                            .arg(md->getDate().toString("MMMM d, yyyy"))
                            .arg(b.getConfirmationNumber()));
    return true;
}

bool DataStore::cancelBooking(int vendorId, int marketDateId) {
    Vendor* vendor = findVendorById(vendorId);
    MarketDate* md = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    for (int i = 0; i < m_bookings.size(); ++i) {
        if (m_bookings[i].getVendorId() == vendorId &&
            m_bookings[i].getMarketDateId() == marketDateId) {
            m_bookings.removeAt(i);
            if (vendor->getCategory() == VendorCategory::Food) md->cancelFood();
            else md->cancelArtisan();
            vendor->addNotification(QString("❌ Booking cancelled for %1")
                                    .arg(md->getDate().toString("MMMM d, yyyy")));
            processWaitlistOnCancellation(marketDateId, vendor->getCategory());
            return true;
        }
    }
    return false;
}

QList<WaitlistEntry> DataStore::getWaitlistForVendor(int vendorId) const {
    QList<WaitlistEntry> result;
    for (const auto& w : m_waitlist) {
        if (w.getVendorId() == vendorId) result.append(w);
    }
    return result;
}

bool DataStore::vendorOnWaitlist(int vendorId, int marketDateId) const {
    for (const auto& w : m_waitlist) {
        if (w.getVendorId() == vendorId && w.getMarketDateId() == marketDateId)
            return true;
    }
    return false;
}

bool DataStore::vendorWaitlistNotified(int vendorId, int marketDateId) const {
    for (const auto& w : m_waitlist) {
        if (w.getVendorId() == vendorId && w.getMarketDateId() == marketDateId)
            return w.isNotified();
    }
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
    Vendor* vendor = findVendorById(vendorId);
    MarketDate* md = findMarketDate(marketDateId);

    // Compliance check — vendor must be compliant to join waitlist
    // (they must be able to book when notified)
    if (!vendor || !vendor->hasAllRequiredDocs()) return false;
    if (!md) return false;

    WaitlistEntry w(m_nextWaitlistId++, vendorId, marketDateId,
                    md->getDate(), vendor->getCategory());
    int pos = getWaitlistPosition(vendorId, marketDateId);  // will be new last position
    // Count existing in queue
    int queueLen = 0;
    for (const auto& existing : m_waitlist) {
        if (existing.getMarketDateId() == marketDateId &&
            existing.getCategory() == vendor->getCategory())
            ++queueLen;
    }
    w.setPosition(queueLen + 1);
    m_waitlist.append(w);
    vendor->addNotification(QString("⏳ Added to waitlist for %1 (Position: %2)")
                            .arg(md->getDate().toString("MMMM d, yyyy"))
                            .arg(queueLen + 1));
    return true;
}

bool DataStore::leaveWaitlist(int vendorId, int marketDateId) {
    Vendor* vendor = findVendorById(vendorId);
    MarketDate* md = findMarketDate(marketDateId);
    if (!vendor || !md) return false;

    for (int i = 0; i < m_waitlist.size(); ++i) {
        if (m_waitlist[i].getVendorId() == vendorId &&
            m_waitlist[i].getMarketDateId() == marketDateId) {
            VendorCategory cat = m_waitlist[i].getCategory();
            m_waitlist.removeAt(i);
            // Reorder positions
            int pos = 1;
            for (auto& w : m_waitlist) {
                if (w.getMarketDateId() == marketDateId && w.getCategory() == cat)
                    w.setPosition(pos++);
            }
            vendor->addNotification(QString("🚫 Removed from waitlist for %1")
                                    .arg(md->getDate().toString("MMMM d, yyyy")));
            return true;
        }
    }
    return false;
}

void DataStore::processWaitlistOnCancellation(int marketDateId, VendorCategory category) {
    for (auto& w : m_waitlist) {
        if (w.getMarketDateId() == marketDateId && w.getCategory() == category &&
            w.getPosition() == 1) {
            Vendor* vendor = findVendorById(w.getVendorId());
            MarketDate* md = findMarketDate(marketDateId);
            if (vendor && md) {
                w.setNotified(true);
                vendor->addNotification(
                    QString("🔔 A stall is now available for %1! You are #1 on the waitlist. Please book your stall.")
                    .arg(md->getDate().toString("MMMM d, yyyy")));
            }
            return;
        }
    }
}