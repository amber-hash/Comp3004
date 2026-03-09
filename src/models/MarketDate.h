#pragma once
#include <QDate>
#include <QString>

class MarketDate {
public:
    MarketDate() = default;
    MarketDate(int id, const QDate& date, int maxFood = 2, int maxArtisan = 2);

    int getId() const { return m_id; }
    QDate getDate() const { return m_date; }
    QString getDateString() const { return m_date.toString("MMMM d, yyyy (dddd)"); }

    int getMaxFood() const { return m_maxFood; }
    int getMaxArtisan() const { return m_maxArtisan; }
    int getBookedFood() const { return m_bookedFood; }
    int getBookedArtisan() const { return m_bookedArtisan; }

    int getAvailableFood() const { return m_maxFood - m_bookedFood; }
    int getAvailableArtisan() const { return m_maxArtisan - m_bookedArtisan; }

    bool hasFoodAvailability() const { return m_bookedFood < m_maxFood; }
    bool hasArtisanAvailability() const { return m_bookedArtisan < m_maxArtisan; }

    void bookFood() { if (m_bookedFood < m_maxFood) ++m_bookedFood; }
    void bookArtisan() { if (m_bookedArtisan < m_maxArtisan) ++m_bookedArtisan; }
    void cancelFood() { if (m_bookedFood > 0) --m_bookedFood; }
    void cancelArtisan() { if (m_bookedArtisan > 0) --m_bookedArtisan; }

private:
    int m_id;
    QDate m_date;
    int m_maxFood;
    int m_maxArtisan;
    int m_bookedFood = 0;
    int m_bookedArtisan = 0;
};
