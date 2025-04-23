#pragma once

#include "payloads/Config.h"
#include "widgets/InputLine.h"
#include <Qontrol>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {

class Settings : public qontrol::Screen {
    Q_OBJECT
public:
    explicit Settings(AccountController *ctrl);

signals:
    void saveSettings(payload::Config);

public slots:
    void loadConfig(payload::Config payload);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

    void actionSave();

private:
    AccountController *m_controller = nullptr;
    bool m_view_init = false;
    QWidget *m_main_widget = nullptr;
    qontrol::widgets::InputLine *m_electrum_url = nullptr;
    qontrol::widgets::InputLine *m_electrum_port = nullptr;
    qontrol::widgets::InputLine *m_nostr_relay = nullptr;
    qontrol::widgets::InputLine *m_nostr_back = nullptr;
    qontrol::widgets::InputLine *m_look_ahead = nullptr;
    QPushButton *m_btn_save = nullptr;
};

} // namespace screen
