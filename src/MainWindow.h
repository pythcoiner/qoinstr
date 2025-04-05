#pragma once

#include "AccountWidget.h"
#include <Qontrol>
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qhash.h>
#include <qnamespace.h>
#include <qtabwidget.h>
#include <qwidget.h>

class AccountWidget;

class MainWindow : public qontrol::Window {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void insertAccount(AccountWidget *account, const QString &name);
    void removeAccount(const QString &name);
    auto accountExists(const QString &name) -> bool;
    void updateTabs();

    auto tabWidget() -> QTabWidget*;
    auto tabs() -> QHash<QString, QWidget*>&;

signals:

private:
    bool m_init = false;
    QTabWidget *m_tab = nullptr;
    QList<QPair<QString, AccountWidget*>> m_tabs;
    QWidget *m_menu_tab = nullptr;

    void initWindow();
};
