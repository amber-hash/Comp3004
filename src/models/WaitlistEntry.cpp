#include "WaitlistEntry.h"

WaitlistEntry::WaitlistEntry(int id, int vendorId, int marketDateId,
                              const QDate& marketDate, VendorCategory category)
    : m_id(id), m_vendorId(vendorId), m_marketDateId(marketDateId),
      m_marketDate(marketDate), m_category(category), m_position(0), m_notified(false) {}
