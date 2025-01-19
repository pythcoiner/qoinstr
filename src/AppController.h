#pragma once

#include <QObject>
#include <Qontrol>
#include <cstdint>

namespace screen {
class Coins;
class Pools;
class Send;
class Receive;
class Settings;
} // namespace screen

class AppController : public Controller {
    Q_OBJECT 
public:
    void loadPanels() override;
    static void init();
    static auto get() -> AppController*;
    static auto window() -> QWidget*;

    static auto coins() -> screen::Coins*;
    static auto pools() -> screen::Pools*;
    static auto send() -> screen::Send*;
    static auto receive() -> screen::Receive*;
    static auto settings() -> screen::Settings*;


public slots:
    void coinsClicked();
    void poolsClicked();
    void sendClicked();
    void receiveClicked();
    void settingsClicked();

    void createPoolWithOutPoint(QString outpoint);
    void createPoolOnRelay(QString relay);
    void joinPool(QString outpoint);
    void joinPoolById(QString id);
    void poolDetails(QString id);

    void showAddressQr(QString addr, uint64_t index, bool change);
    void copyAddress(QString addr);
    void saveAddressLabel(QString addr, QString label);
    void generateAddress();
    
    void listpools();
};
