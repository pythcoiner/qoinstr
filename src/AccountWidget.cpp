#include "AccountWidget.h"
#include "AccountController.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include <qboxlayout.h>
#include <qlogging.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qvariant.h>

AccountWidget::AccountWidget(const QString &account, QWidget *parent) : QWidget(parent) {
    m_controller = new AccountController(account, this);
    initWidget();
    m_controller->loadPanels();
    m_controller->cmdLoadConfig();
}

void AccountWidget::initWidget() {
    if (m_init)
        return;
    if (m_side_menu == nullptr) {
        auto *coins = new QPushButton("Coins");
        coins->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto *pools = new QPushButton("Pools");
        pools->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto *send = new QPushButton("Send");
        send->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto *receive = new QPushButton("Receive");
        receive->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto *settings = new QPushButton("Settings");
        settings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_side_menu = (new qontrol::Column(this))->push(coins)->push(pools)->push(send)->push(receive)->push(settings);
        m_side_menu->setFixedWidth(150);

        connect(coins, &QPushButton::clicked, m_controller, &AccountController::coinsClicked);
        connect(pools, &QPushButton::clicked, m_controller, &AccountController::poolsClicked);
        connect(send, &QPushButton::clicked, m_controller, &AccountController::sendClicked);
        connect(receive, &QPushButton::clicked, m_controller, &AccountController::receiveClicked);
        connect(settings, &QPushButton::clicked, m_controller, &AccountController::settingsClicked);
    }

    if (m_screen_container == nullptr) {
        this->m_screen_container = new QWidget(this);
        m_screen_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    auto *mainLayout = (new qontrol::Row(this))->push(m_side_menu)->push(m_screen_container);

    this->setLayout(mainLayout->layout());
    m_init = true;
}

AccountWidget::~AccountWidget() = default;

void AccountWidget::loadPanel(qontrol::Panel *panel) {
    if (panel == nullptr)
        return;

    if (m_current_panel != nullptr) {
        auto *previousScreen = takeScreen();
        if (previousScreen != nullptr) {

            // m_current_panel takes ownership of unloaded screen
            m_current_panel->setScreen(previousScreen);

            previousScreen->onUnload();
        }
        if (m_current_screen != nullptr) {
            m_screen_container->layout()->removeWidget(m_current_screen);
        }
    }

    m_current_panel = panel;
    m_current_panel->connectScreen();
    setScreen(panel->widget());
    m_current_panel->widget()->setVisible(true);
}

void AccountWidget::setScreen(qontrol::Screen *screen) {
    if (m_screen_container->layout() == nullptr) {
        m_screen_container->setLayout(new QHBoxLayout);
    }
    m_screen_container->layout()->addWidget(screen);
    m_current_screen = screen;
    m_current_screen->setVisible(true);
}

auto AccountWidget::takeScreen() -> qontrol::Screen * {
    auto *screen = m_current_screen;
    m_current_screen = nullptr;
    screen->setVisible(false);
    return screen;
}

// load a qontrol::Screen::layout() into the screen container
void AccountWidget::loadLayout(QLayout *layout) {
    m_screen_container->setLayout(layout);
}

// return a raw pointer to the QWidget containing the qontrol::Screen
auto AccountWidget::screenContainer() -> QWidget * {
    return m_screen_container;
}

auto AccountWidget::controller() -> AccountController * {
    return m_controller;
}
