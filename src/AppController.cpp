#include "AppController.h"
#include "screens/Coins.h"
#include "screens/Pools.h"
#include "screens/Send.h"
#include "screens/Receive.h"
#include "screens/Settings.h"
#include <qlogging.h>


void AppController::loadPanels() {
    auto *coins = new qontrol::Panel(new screen::Coins(), "coins");
    this->insertPanel(coins);

    auto *pools = new qontrol::Panel(new screen::Pools(), "pools");
    this->insertPanel(pools);

    auto *send = new qontrol::Panel(new screen::Send(), "send");
    this->insertPanel(send);

    auto *receive = new qontrol::Panel(new screen::Receive(), "receive");
    this->insertPanel(receive);

    auto *settings = new qontrol::Panel(new screen::Settings(), "settings");
    this->insertPanel(settings);

    this->loadPanel("coins");
}

void AppController::coinsClicked() {
    this->loadPanel("coins");
}

void AppController::poolsClicked() {
    this->loadPanel("pools");
}

void AppController::sendClicked() {
    this->loadPanel("send");
}

void AppController::receiveClicked() {
    this->loadPanel("receive");
}

void AppController::settingsClicked() {
    this->loadPanel("settings");
}

auto AppController::get() -> AppController* {
    auto *ctrl = Controller::get();
    auto *controller = dynamic_cast<AppController*>(ctrl);
    return controller;
}

auto AppController::coins() -> screen::Coins* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Coins*>(ctrl->screen("coins").value());
    return screen;
}

auto AppController::pools() -> screen::Pools* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Pools*>(ctrl->screen("pools").value());
    return screen;
}

auto AppController::send() -> screen::Send* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Send*>(ctrl->screen("send").value());
    return screen;
}

auto AppController::receive() -> screen::Receive* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Receive*>(ctrl->screen("receive").value());
    return screen;
}

auto AppController::settings() -> screen::Settings* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Settings*>(ctrl->screen("settings").value());
    return screen;
}

void AppController::init() {
    if (Controller::isInit()) {
        qFatal() << "Controller have already been initiated!";
    }
    Controller::init(new AppController);
}

auto AppController::window() -> QWidget* {

    return Controller::window();
}

void AppController::createPoolWithOutPoint(QString outpoint) {
    qDebug() << "AppController::createPoolWithOutPoint("<<outpoint<<")";
}

void AppController::createPoolOnRelay(QString relay){
    qDebug() << "AppController::createPoolOnRelay("<<relay<<")";
}

void AppController::joinPool(QString outpoint) {
    qDebug() << "AppController::joinPool("<<outpoint<<")";
}

void AppController::joinPoolById(QString id) {
    qDebug() << "AppController::joinPoolById("<<id<<")";
}

void AppController::poolDetails(QString id) {
    qDebug() << "AppController::poolDetails("<<id<<")";
}

void AppController::showAddressQr(QString addr, uint64_t index, bool change) {
    qDebug() << "AppController::showAddressQr("<<addr<<")";
}

void AppController::copyAddress(QString addr) {
    qDebug() << "AppController::copyAddress("<<addr<<")";
}

void AppController::saveAddressLabel(QString addr, QString label) {
    qDebug() << "AppController::saveAddressLabel("<<label<<")";
}

void AppController::generateAddress() {
    qDebug() << "AppController::generateAddress()";
}

void AppController::listpools() {
    qDebug() << "AppController::listpools()";
}
