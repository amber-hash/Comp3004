#include "Booking.h"

Booking::Booking(int id, int vendorId, int marketDateId, const QDate& marketDate)
    : m_id(id), m_vendorId(vendorId), m_marketDateId(marketDateId), m_marketDate(marketDate) {
    m_confirmationNumber = QString("HM-%1-%2").arg(marketDate.toString("yyyyMMdd")).arg(id);
}
