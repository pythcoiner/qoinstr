#pragma once

#include "include/cpp_joinstr.h"
#include <QObject>
#include <Qontrol>
#include <qobject.h>
#include <qsystemtrayicon.h>


namespace screen {
class Coins;
class Pools;
class Send;
class Receive;
class Settings;
} // namespace screen

class AppController : public qontrol::Controller {
    Q_OBJECT 
public:
    AppController();
    static void init();
    static auto get() -> AppController*;

public slots:
    void initState();
    void addAccount(const QString &name);
    void removeAccount(const QString &account);

    // OS Notifications
    void osMessage(QString title, QString msg, int delay=10000);
    void osInfo(QString title, QString msg, int delay=10000);
    void osWarning(QString title, QString msg, int delay=10000);
    void osCritical(QString title, QString msg, int delay=10000);

private:
    QSystemTrayIcon *m_tray_icon = nullptr;

};
