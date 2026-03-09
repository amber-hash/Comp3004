#include "LoginScreen.h"
#include "StyleSheet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

LoginScreen::LoginScreen(MarketManager* manager, QWidget* parent)
    : QWidget(parent), m_manager(manager) {
    setupUI();
}

void LoginScreen::setupUI() {
    QVBoxLayout* pageLayout = new QVBoxLayout(this);
    pageLayout->setAlignment(Qt::AlignCenter);
    pageLayout->setContentsMargins(40, 40, 40, 40);

    QFrame* card = new QFrame();
    card->setStyleSheet(R"(
        QFrame { background-color: #FAF7F0; border: 1px solid #D9CDB8; border-radius: 12px; }
    )");
    card->setFixedWidth(420);

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(18);
    cardLayout->setContentsMargins(40, 36, 40, 36);

    QLabel* logoLabel = new QLabel("🌿");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("font-size: 42px; border: none;");

    QLabel* titleLabel = new QLabel("HintonMarket");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        font-size: 28px; font-weight: bold; color: #3D6B4F;
        font-family: "Georgia", serif; border: none;
    )");

    QLabel* subtitleLabel = new QLabel("Hintonville Farmers Market Management System");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setStyleSheet("font-size: 12px; color: #7A5C3A; font-family: Georgia, serif; border: none;");

    QFrame* divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background: #D9CDB8; border: none; max-height: 1px;");

    QLabel* promptLabel = new QLabel("Enter your username or name to sign in:");
    promptLabel->setStyleSheet("font-size: 13px; color: #5C4A30; border: none;");

    m_usernameEdit = new QLineEdit();
    m_usernameEdit->setPlaceholderText("e.g. freshharvest, operator, admin...");
    m_usernameEdit->setStyleSheet(StyleSheet::lineEdit());
    m_usernameEdit->setMinimumHeight(40);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginScreen::onLoginClicked);

    m_errorLabel = new QLabel("");
    m_errorLabel->setStyleSheet(R"(
        color: #B84040; font-size: 12px; border: none;
        padding: 4px 8px; background: #FCE8E8; border-radius: 4px;
    )");
    m_errorLabel->hide();

    m_loginBtn = new QPushButton("Sign In →");
    m_loginBtn->setStyleSheet(StyleSheet::primaryButton());
    m_loginBtn->setMinimumHeight(42);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginScreen::onLoginClicked);

    // Hint box
    QFrame* hintBox = new QFrame();
    hintBox->setStyleSheet("QFrame { background: #EAF2EC; border: 1px solid #B8D8C0; border-radius: 6px; }");
    QVBoxLayout* hintLayout = new QVBoxLayout(hintBox);
    hintLayout->setContentsMargins(12, 10, 12, 10);
    hintLayout->setSpacing(3);
    QLabel* hintTitle = new QLabel("Test Accounts");
    hintTitle->setStyleSheet("font-weight: bold; color: #3D6B4F; font-size: 12px; border: none;");
    QLabel* hintContent = new QLabel(
        "<b>Food Vendors:</b> freshharvest, sunrisebakery, greenvalley, maplesyrup<br>"
        "<b>Artisan Vendors:</b> claycreations, woodcraft, silkthread, jewelrybox<br>"
        "<b>Staff:</b> operator, admin"
    );
    hintContent->setStyleSheet("font-size: 11px; color: #3D6B4F; border: none;");
    hintContent->setWordWrap(true);
    hintLayout->addWidget(hintTitle);
    hintLayout->addWidget(hintContent);

    cardLayout->addWidget(logoLabel);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(subtitleLabel);
    cardLayout->addWidget(divider);
    cardLayout->addWidget(promptLabel);
    cardLayout->addWidget(m_usernameEdit);
    cardLayout->addWidget(m_errorLabel);
    cardLayout->addWidget(m_loginBtn);
    cardLayout->addSpacing(4);
    cardLayout->addWidget(hintBox);

    pageLayout->addWidget(card, 0, Qt::AlignCenter);

    setStyleSheet(StyleSheet::global() + R"(
        LoginScreen { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
            stop:0 #F0EBD8, stop:1 #E0D8C0); }
    )");
}

void LoginScreen::onLoginClicked() {
    QString input = m_usernameEdit->text().trimmed();
    if (input.isEmpty()) {
        m_errorLabel->setText("Please enter your username or name.");
        m_errorLabel->show();
        return;
    }

    // Delegate to MarketManager — UI never touches DataStore
    User* user = m_manager->login(input);

    if (!user) {
        m_errorLabel->setText(
            QString("No account found for \"%1\". Please check your username.").arg(input));
        m_errorLabel->show();
        return;
    }

    m_errorLabel->hide();
    m_usernameEdit->clear();
    emit loginSuccess(user);
}
