#include "Settings.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include "common.h"
#include "payloads/Config.h"
#include "screens/common.h"
#include "widgets/Input.h"
#include "widgets/InputLine.h"
#include <cstdint>
#include <qlabel.h>
#include <qpushbutton.h>

namespace screen {

Settings::Settings() {
    this->init();
    this->doConnect();
    this->view();
}

void Settings::init() {
    view();
    doConnect();
}

void Settings::doConnect() {
    auto *ctrl = AppController::get();
    connect(m_btn_save, &QPushButton::clicked, this, &Settings::actionSave, qontrol::UNIQUE);
    connect(this, &Settings::saveSettings, ctrl, &AppController::cmdSaveConfig, qontrol::UNIQUE);
    connect(ctrl, &AppController::loadConfig, this, &Settings::loadConfig, qontrol::UNIQUE);
}

void Settings::actionSave() {
    auto payload = payload::Config();
    payload.electrum_url = m_electrum_url->value().toString();
    payload.electrum_port = QString::number(m_electrum_port->value().toInt());
    payload.nostr_relay = m_nostr_relay->value().toString();
    payload.nostr_back = QString::number(m_nostr_back->value().toInt());
    emit saveSettings(payload);
}

void Settings::loadConfig(payload::Config payload) { // NOLINT(performance-unnecessary-value-param)
    if (!m_view_init) return;
    m_electrum_url->setValue(payload.electrum_url);
    m_electrum_port->setValue(payload.electrum_port);
    m_nostr_relay->setValue(payload.nostr_relay);
    m_nostr_back->setValue(payload.nostr_back);
}

void Settings::view() {
    m_electrum_url = (new qontrol::widgets::InputLine("elecrum_url"))
        ->label("Electrum url:")
        ->input(qontrol::widgets::InputType::String);

    m_electrum_port = (new qontrol::widgets::InputLine("electrum_port"))
        ->label("Electrum port:")
        ->input(qontrol::widgets::InputType::Int);

    m_nostr_relay = (new qontrol::widgets::InputLine("nostr_relay"))
        ->label("Nostr relay url:")
        ->input(qontrol::widgets::InputType::String);

    m_nostr_back = (new qontrol::widgets::InputLine("nostr_back"))
        ->label("Nostr scan back:")
        ->input(qontrol::widgets::InputType::Int);

    m_btn_save = new QPushButton("Save");

    auto *saveRow = (new qontrol::Row)
        ->pushSpacer()
        ->push(m_btn_save)
        ->pushSpacer()
        ;
    
    auto *col = (new qontrol::Column)
        ->pushSpacer(50)
        ->push(m_electrum_url)
        ->pushSpacer(V_SPACER)
        ->push(m_electrum_port)
        ->pushSpacer(V_SPACER)
        ->push(m_nostr_relay)
        ->pushSpacer(V_SPACER)
        ->push(m_nostr_back)
        ->pushSpacer(30)
        ->push(saveRow)
        ->pushSpacer()
        ;

    auto *row = (new qontrol::Row)
        ->pushSpacer()
        ->push(col)
        ->pushSpacer()
        ->pushSpacer()
        ;

    m_main_widget = margin(row);
    this->setLayout(m_main_widget->layout());
    m_view_init = true;
}

}

