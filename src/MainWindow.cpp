#include "MainWindow.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include <qpushbutton.h>
#include <qsizepolicy.h>

MainWindow::MainWindow(QWidget *parent) : Window(parent) {
    AppController::init();
    AppController::get()->start(this);
    this->initWindow();
    AppController::get()->loadPanels();
}

void MainWindow::initWindow() {
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
        m_side_menu = (new Column(this))
            ->push(coins)
            ->push(pools)
            ->push(send)
            ->push(receive)
            ->push(settings)
            ;
        m_side_menu->setFixedWidth(150);

        connect(coins, &QPushButton::clicked, AppController::get(), &AppController::coinsClicked);
        connect(pools, &QPushButton::clicked, AppController::get(), &AppController::poolsClicked);
        connect(send, &QPushButton::clicked, AppController::get(), &AppController::sendClicked);
        connect(receive, &QPushButton::clicked, AppController::get(), &AppController::receiveClicked);
        connect(settings, &QPushButton::clicked, AppController::get(), &AppController::settingsClicked);
    }

    if (m_panel_widget == nullptr) {
        this-> m_panel_widget = new QWidget(this);
        m_panel_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    auto *mainLayout = (new Row(this))
        ->push(m_side_menu)
        ->push(m_panel_widget)
        ;

    // avoid setCentralWidget() to remove the previous widget
    auto *taken = this->takeCentralWidget(); // NOLINT
    this->setCentralWidget(mainLayout->widget());
}

MainWindow::~MainWindow() = default;

void MainWindow::loadLayout(QLayout *layout) {
    m_panel_widget->setLayout(layout);
}

auto MainWindow::centralWidget() -> QWidget* {
    return m_panel_widget;
}

void MainWindow::loadPanel(QWidget *widget) {
    m_panel_widget = widget;
    this->initWindow();
}

auto MainWindow::takePanel() -> QWidget* {
    auto *panel = m_panel_widget;
    return panel;
}
