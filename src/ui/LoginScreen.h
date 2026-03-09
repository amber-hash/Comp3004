#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "../models/User.h"
#include "../controller/MarketManager.h"

class LoginScreen : public QWidget {
    Q_OBJECT
public:
    // Takes MarketManager so it can call manager->login()
    explicit LoginScreen(MarketManager* manager, QWidget* parent = nullptr);

signals:
    void loginSuccess(User* user);

private slots:
    void onLoginClicked();

private:
    void setupUI();

    MarketManager* m_manager;
    QLineEdit*     m_usernameEdit;
    QLabel*        m_errorLabel;
    QPushButton*   m_loginBtn;
};
