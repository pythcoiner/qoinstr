#include "CreateAccount.h"
#include "AppController.h"
#include "Column.h"
#include "Modal.h"
#include "Row.h"
#include "common.h"
#include "include/cpp_joinstr.h"
#include "screens/common.h"
#include "widgets/ComboLine.h"

#include <Qontrol>
#include <cmath>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qwidget.h>

namespace modal {

CreateAccount::CreateAccount() {
    this->setWindowTitle("Create new wallet");
    this->setFixedSize(680, 400);

    // Network
    auto networks = QStringList();
    networks.append("Bitcoin");
    networks.append("Signet");
    networks.append("Testnet");
    networks.append("Regtest");
    m_network = new widget::ComboLine("Network: ", networks);
    m_network->label()->setFixedWidth(LABEL_WIDTH);
    m_network->combo()->setFixedWidth(INPUT_WIDTH);

    // Name
    m_name = new widget::InputLine("Wallet name:");
    m_name->label()->setFixedWidth(LABEL_WIDTH);
    m_name->input()->setFixedWidth(INPUT_WIDTH);

    // Mnemonic
    m_mnemonic = new widget::InputLine("Mnemonic:");
    m_mnemonic->label()->setFixedWidth(LABEL_WIDTH);
    m_mnemonic->input()->setFixedWidth(INPUT_WIDTH);
    m_generate_btn = new QPushButton("Generate");
    connect(m_generate_btn, &QPushButton::clicked, this,
            &CreateAccount::onGenerateMnemonic, qontrol::UNIQUE);
    m_mnemonic->pushSpacer(H_SPACER)->push(m_generate_btn);

    // Create btn
    m_create_btn = new QPushButton("Create wallet");
    connect(m_create_btn, &QPushButton::clicked, this,
            &CreateAccount::onCreateAccount, qontrol::UNIQUE);
    connect(this, &CreateAccount::createAccount, AppController::get(),
            &AppController::createAccount, qontrol::UNIQUE);
    auto *btn = (new qontrol::Row)
                    ->pushSpacer()
                    ->push(m_create_btn)
                    ->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->pushSpacer(40)
                    ->push(m_network)
                    ->pushSpacer(V_SPACER)
                    ->push(m_name)
                    ->pushSpacer(V_SPACER)
                    ->push(m_mnemonic)
                    ->pushSpacer(V_SPACER)
                    ->push(btn)
                    ->pushSpacer();

    auto *row = (new qontrol::Row)->pushSpacer()->push(col)->pushSpacer();
    this->setMainWidget(row);
}

void CreateAccount::onCreateAccount() {
    // check mnemonic is a valid bip39 mnemonic
    auto mnemo = mnemonic_from_string(
        m_mnemonic->input()->text().toStdString());
    if (!mnemo->is_ok()) {
        auto *modal = new qontrol::Modal(
            "Invalid mnemonic!",
            "Your mnemonic is not a valid BIP39 mnemonic!");
        AppController::execModal(modal);
        return;
    }

    // TODO: check the no account w/ this name already exists
    auto name = m_name->input()->text();
    if (name.contains(" ")) {
        auto *modal = new qontrol::Modal(
            "Invalid Name!",
            "The name of the wallet must not contains any space!");
        AppController::execModal(modal);
        return;
    }

    if (config_exists(name.toStdString())) {
        auto *modal = new qontrol::Modal(
            "Wallet already exists!", "There is an existing configuration file "
                                      "for a wallet with the same name!");
        AppController::execModal(modal);
        return;
    }

    Network network; // NOLINT
    auto networkStr = m_network->combo()->currentText();
    if (networkStr == "Bitcoin") {
        network = Network::Bitcoin;
    } else if (networkStr == "Signet") {
        network = Network::Signet;
    } else if (networkStr == "Regtest") {
        network = Network::Regtest;
    } else if (networkStr == "Testnet") {
        network = Network::Testnet;
    } else {

        auto comment = QString("%1 is not a valid network!").arg(networkStr);
        auto *modal = new qontrol::Modal("Invalid network!", comment);
        AppController::execModal(modal);
        return;
    }

    emit createAccount(m_name->input()->text(), m_mnemonic->input()->text(),
                       network);
    close();
}

void CreateAccount::onGenerateMnemonic() {
    auto mnemonic = QString(generate_mnemonic().c_str());
    m_mnemonic->input()->setText(mnemonic);
    m_generate_btn->setEnabled(false);
}

} // namespace modal
