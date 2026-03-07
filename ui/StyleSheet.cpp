#include "StyleSheet.h"

const QString StyleSheet::GREEN      = "#3D6B4F";
const QString StyleSheet::GREEN_DARK = "#2A4D38";
const QString StyleSheet::CREAM      = "#F8F3E8";
const QString StyleSheet::BROWN      = "#7A5C3A";
const QString StyleSheet::BROWN_LIGHT = "#C4A882";
const QString StyleSheet::TEXT_DARK  = "#2B2016";
const QString StyleSheet::TEXT_MID   = "#5C4A30";
const QString StyleSheet::DANGER     = "#B84040";
const QString StyleSheet::WARNING    = "#C47C20";
const QString StyleSheet::BG         = "#FDFAF4";

QString StyleSheet::global() {
    return R"(
        QMainWindow, QDialog {
            background-color: #FDFAF4;
        }
        QWidget {
            font-family: "Georgia", "Times New Roman", serif;
            color: #2B2016;
            font-size: 13px;
        }
        QLabel {
            color: #2B2016;
        }
        QScrollArea {
            border: none;
            background: transparent;
        }
        QScrollBar:vertical {
            background: #EDE7D9;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #C4A882;
            border-radius: 4px;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QTabWidget::pane {
            border: 1px solid #D9CDB8;
            background: #FDFAF4;
            border-radius: 4px;
        }
        QTabBar::tab {
            background: #EDE7D9;
            color: #5C4A30;
            padding: 8px 20px;
            border: 1px solid #D9CDB8;
            border-bottom: none;
            font-family: "Georgia", serif;
            font-size: 13px;
        }
        QTabBar::tab:selected {
            background: #3D6B4F;
            color: #F8F3E8;
            font-weight: bold;
        }
        QTabBar::tab:hover:!selected {
            background: #D9CDB8;
        }
        QGroupBox {
            border: 1px solid #D9CDB8;
            border-radius: 6px;
            margin-top: 12px;
            padding: 8px;
            background: #FAF7F0;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 6px;
            color: #3D6B4F;
            font-weight: bold;
            font-size: 13px;
        }
        QMessageBox {
            background-color: #FDFAF4;
        }
        QMessageBox QLabel {
            font-size: 13px;
        }
    )";
}

QString StyleSheet::primaryButton() {
    return R"(
        QPushButton {
            background-color: #3D6B4F;
            color: #F8F3E8;
            border: none;
            border-radius: 5px;
            padding: 9px 20px;
            font-family: "Georgia", serif;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #4E8563; }
        QPushButton:pressed { background-color: #2A4D38; }
        QPushButton:disabled { background-color: #A8C4B0; color: #D0E5D8; }
    )";
}

QString StyleSheet::secondaryButton() {
    return R"(
        QPushButton {
            background-color: transparent;
            color: #3D6B4F;
            border: 2px solid #3D6B4F;
            border-radius: 5px;
            padding: 7px 18px;
            font-family: "Georgia", serif;
            font-size: 13px;
        }
        QPushButton:hover { background-color: #EAF2EC; }
        QPushButton:pressed { background-color: #D4E8DA; }
    )";
}

QString StyleSheet::dangerButton() {
    return R"(
        QPushButton {
            background-color: #B84040;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 9px 20px;
            font-family: "Georgia", serif;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #D04848; }
        QPushButton:pressed { background-color: #8E3030; }
        QPushButton:disabled { background-color: #D8A0A0; }
    )";
}

QString StyleSheet::card() {
    return R"(
        QFrame {
            background-color: #FAF7F0;
            border: 1px solid #D9CDB8;
            border-radius: 8px;
        }
    )";
}

QString StyleSheet::tableWidget() {
    return R"(
        QTableWidget {
            background-color: #FDFAF4;
            gridline-color: #E8DFCE;
            border: 1px solid #D9CDB8;
            border-radius: 4px;
            font-size: 13px;
        }
        QTableWidget::item {
            padding: 6px 10px;
            border-bottom: 1px solid #EDE7D9;
        }
        QTableWidget::item:selected {
            background-color: #D4E8DA;
            color: #2B2016;
        }
        QHeaderView::section {
            background-color: #3D6B4F;
            color: #F8F3E8;
            padding: 8px 10px;
            border: none;
            font-weight: bold;
            font-family: "Georgia", serif;
            font-size: 12px;
        }
    )";
}

QString StyleSheet::lineEdit() {
    return R"(
        QLineEdit {
            background-color: #FDFAF4;
            border: 1.5px solid #C4A882;
            border-radius: 5px;
            padding: 8px 12px;
            font-size: 13px;
            color: #2B2016;
        }
        QLineEdit:focus {
            border-color: #3D6B4F;
            background: white;
        }
    )";
}

QString StyleSheet::sectionHeader() {
    return R"(
        QLabel {
            font-size: 15px;
            font-weight: bold;
            color: #3D6B4F;
            padding-bottom: 4px;
            border-bottom: 2px solid #C4A882;
        }
    )";
}

QString StyleSheet::badge(const QString& color) {
    return QString(R"(
        QLabel {
            background-color: %1;
            color: white;
            border-radius: 10px;
            padding: 2px 10px;
            font-size: 11px;
            font-weight: bold;
            font-family: "Arial", sans-serif;
        }
    )").arg(color);
}
