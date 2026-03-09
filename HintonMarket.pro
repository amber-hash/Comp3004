QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = HintonMarket
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/models/User.cpp \
    src/models/Vendor.cpp \
    src/models/MarketDate.cpp \
    src/models/Booking.cpp \
    src/models/WaitlistEntry.cpp \
    src/models/ComplianceDoc.cpp \
    src/data/DataStore.cpp \
    src/controller/MarketManager.cpp \
    src/ui/MainWindow.cpp \
    src/ui/LoginScreen.cpp \
    src/ui/VendorDashboard.cpp \
    src/ui/MarketScheduleScreen.cpp \
    src/ui/BookingDialog.cpp \
    src/ui/WaitlistDialog.cpp \
    src/ui/StyleSheet.cpp

HEADERS += \
    src/models/User.h \
    src/models/Vendor.h \
    src/models/MarketDate.h \
    src/models/Booking.h \
    src/models/WaitlistEntry.h \
    src/models/ComplianceDoc.h \
    src/data/DataStore.h \
    src/controller/MarketManager.h \
    src/ui/MainWindow.h \
    src/ui/LoginScreen.h \
    src/ui/VendorDashboard.h \
    src/ui/MarketScheduleScreen.h \
    src/ui/BookingDialog.h \
    src/ui/WaitlistDialog.h \
    src/ui/StyleSheet.h
