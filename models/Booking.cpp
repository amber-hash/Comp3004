#include "Booking.h"

Booking::Booking(int id, int vendorId, int marketDateId, const QDate& marketDate)
    : m_id(id), m_vendorId(vendorId), m_marketDateId(marketDateId), m_marketDate(marketDate)
{
    // Build confirmation number after all members are initialized
    // Format: HM-YYYYMMDD-<id>
    m_confirmationNumber = QString("HM-%1-%2")
                               .arg(m_marketDate.toString("yyyyMMdd"))
                               .arg(m_id);
}
