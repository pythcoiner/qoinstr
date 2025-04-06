#pragma once

#include "Panel.h"
#include "Screen.h"
#include <Qontrol>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

class AccountController;

class AccountWidget : public QWidget {
    Q_OBJECT

public:
    AccountWidget(const QString &account, QWidget *parent = nullptr);
    ~AccountWidget() override;
    void loadLayout(QLayout *layout);
    auto screenContainer() -> QWidget*;
    auto takeScreen() -> qontrol::Screen*;
    void setScreen(qontrol::Screen* screen);
    void loadPanel(qontrol::Panel* layout);
    auto controller() -> AccountController*;

signals:

private:
    AccountController *m_controller = nullptr;
    bool m_init = false;
    qontrol::Column *m_side_menu = nullptr;
    // NOTE: m_screen_container is a QWidget that will contain the qontrol::Screen
    QWidget *m_screen_container = nullptr;
    qontrol::Panel *m_current_panel = nullptr;
    qontrol::Screen *m_current_screen = nullptr;

    void initWidget();
};
