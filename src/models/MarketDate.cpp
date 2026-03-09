#include "MarketDate.h"

MarketDate::MarketDate(int id, const QDate& date, int maxFood, int maxArtisan)
    : m_id(id), m_date(date), m_maxFood(maxFood), m_maxArtisan(maxArtisan),
      m_bookedFood(0), m_bookedArtisan(0) {}
