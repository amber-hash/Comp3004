#pragma once
#include <QString>

class StyleSheet {
public:
    static QString global();
    static QString primaryButton();
    static QString secondaryButton();
    static QString dangerButton();
    static QString card();
    static QString badge(const QString& color);
    static QString tableWidget();
    static QString lineEdit();
    static QString sectionHeader();

    // Colors
    static const QString GREEN;
    static const QString GREEN_DARK;
    static const QString CREAM;
    static const QString BROWN;
    static const QString BROWN_LIGHT;
    static const QString TEXT_DARK;
    static const QString TEXT_MID;
    static const QString DANGER;
    static const QString WARNING;
    static const QString BG;
};
