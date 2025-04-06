#include "MainWindow.h"
#include "AppController.h"
#include "screens/MenuTab.h"
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qtabwidget.h>
#include <qwidget.h>

MainWindow::MainWindow(QWidget *parent) : Window(parent) {
    AppController::init();
    this->initWindow();
    AppController::get()->start(this);
    AppController::get()->initState();
}

void MainWindow::initWindow() {
    if (m_init) return;

    m_menu_tab = new screen::MenuTab(this);

    m_tab = new QTabWidget(this);

    updateTabs();

    this->setCentralWidget(m_tab);
    m_init = true;
}

auto MainWindow::accountExists(const QString &name) -> bool {
    if (m_tabs.isEmpty()) return false;
    for (const auto& tab : m_tabs) {
        if (tab.first == name) return true;
    }
    return false;
}

void MainWindow::insertAccount(AccountWidget *account, const QString &name) {

    if (!accountExists(name)) {
        m_tabs.append(QPair(name, account));
        updateTabs();
    } else {
        qCritical() << "Tab for account" << name << "already exists!";
    }
}

void MainWindow::removeAccount(const QString &name) {
    auto exists = false;
    int index = 0;
    for (int i=0; i<m_tabs.size(); ++i) {
        if (m_tabs.at(i).first == name) {
            index = i;
            exists = true;
        }
    }

    if (!exists) return;
    m_tabs.removeAt(index);
}

void MainWindow::updateTabs() {
    for (const auto& tab : m_tabs) {
        m_tab->addTab(tab.second, tab.first);
    }
    m_tab->addTab(m_menu_tab, "+");
}

MainWindow::~MainWindow() = default;

