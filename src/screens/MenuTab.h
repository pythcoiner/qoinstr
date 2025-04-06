#pragma once

#include "payloads/Config.h"
#include "widgets/InputLine.h"
#include <Qontrol>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {

class MenuTab : public qontrol::Screen {
    Q_OBJECT
public:
    explicit MenuTab(QWidget *parent);

signals:
    void openAccount(const QString &name);
    void createAccount();

public slots:
    void updateAccounts(QList<QString> accounts);
    void onAccountCreated(const QString &name);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
    void addAccount(const QString &account);

private:
    QWidget *m_main_widget = nullptr;
    QHash<QString, QPushButton*> m_accounts;
    QPushButton *m_btn_create = nullptr;
};

}
