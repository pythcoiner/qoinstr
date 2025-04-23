#include "MenuTab.h"
#include "AccountController.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include "common.h"
#include "screens/common.h"
#include <qpushbutton.h>

namespace screen {

MenuTab::MenuTab(QWidget *parent) {
    this->setParent(parent);
    this->init();
    this->view();
    this->doConnect();
}

void MenuTab::init() {
    m_btn_create = new QPushButton("Create new wallet");
    m_btn_create->setFixedWidth(INPUT_WIDTH * 2);
    auto font = m_btn_create->font();
    font.setPixelSize(30);
    m_btn_create->setFont(font);
}

void MenuTab::doConnect() {
    connect(m_btn_create, &QPushButton::clicked, this, &MenuTab::createAccount, qontrol::UNIQUE);
    auto *ctrl = AppController::get();
    connect(ctrl, &AppController::accountList, this, &MenuTab::updateAccounts, qontrol::UNIQUE);
    connect(this, &MenuTab::createAccount, ctrl, &AppController::onCreateAccount, qontrol::UNIQUE);
    connect(this, &MenuTab::openAccount, ctrl, &AppController::addAccount, qontrol::UNIQUE);
    connect(ctrl, &AppController::accountCreated, this, &MenuTab::onAccountCreated, qontrol::UNIQUE);
}

void MenuTab::updateAccounts(QList<QString> accounts) { // NOLINT(performance-unnecessary-value-param)
    qDebug() << "MenuTab::updateAccounts(" << accounts << ")";

    // add missing accounts
    for (const auto &account : accounts) {
        addAccount(account);
    }

    // remove deleted accounts
    for (const auto &name : m_accounts.keys()) {
        if (!accounts.contains(name)) {
            m_accounts.remove(name);
        }
    }

    // update the view
    view();
}

void MenuTab::onAccountCreated(const QString &name) {
    addAccount(name);
    view();
}

void MenuTab::addAccount(const QString &account) {
    if (!m_accounts.contains(account)) {
        auto btnText = QString("Open ") + account;
        auto *btn = new QPushButton(btnText);
        btn->setFixedWidth(INPUT_WIDTH * 2);
        auto font = btn->font();
        font.setPixelSize(30);
        btn->setFont(font);
        connect(btn, &QPushButton::clicked, [this, account]() { emit this->openAccount(account); });
        m_accounts.insert(account, btn);
    }
}

void MenuTab::view() {

    auto *col = (new qontrol::Column)->pushSpacer();

    for (auto *btn : m_accounts) {
        col->push(btn)->pushSpacer(V_SPACER * 2);
    }

    col->push(m_btn_create)->pushSpacer();

    auto *row = (new qontrol::Row)->pushSpacer()->push(col)->pushSpacer();

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(row);
    delete oldWidget;

    delete this->layout();
    this->setLayout(m_main_widget->layout());
}

} // namespace screen
