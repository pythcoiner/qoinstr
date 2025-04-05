#include "CreatePool.h"
#include "AccountController.h"
#include "AppController.h"
#include "Column.h"
#include "Modal.h"
#include "Row.h"
#include "screens/common.h"

#include <Qontrol>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qwidget.h>

const int CP_LABEL_WIDTH = 120;
const int CP_INPUT_WIDTH = 100;
const int CP_V_SPACER =5;
const int CP_L_SPACER = 205;
const int VB_FEE = 150;

namespace modal {
using qontrol::Modal;

CreatePool::CreatePool(const payload::Coin &coin, AccountController *ctrl) {
  this->setWindowTitle("Create Pool");
  this->setFixedSize(680, 400);

  m_coin = payload::Coin(coin);
    m_controller = ctrl;

  m_peers_validator = new QIntValidator(2, 10, this);
  m_fees_validator = new QIntValidator(1, 1000, this);
  m_timeout_validator = new QIntValidator(1, 240, this);
  m_denom_validator = new QDoubleValidator(0.0001, 10.0, 5, this);

  // Title
  auto relay = m_controller->relay();
  auto titleStr = QString("Create a new Joinstr Pool on ");
  titleStr = titleStr + relay + QString(" ?");
  auto *title = new QLabel(titleStr);
  auto font = title->font();
  font.setPointSize(16);
  title->setFont(font);

  auto *titleRow = (new qontrol::Row)->pushSpacer()->push(title)->pushSpacer();

  // Coin
  auto *outpoint = new QLineEdit();
  outpoint->setFixedWidth(500);
  outpoint->setText(m_coin.outpoint);
  outpoint->setEnabled(false);

  double btcAmount = coin.value;
  btcAmount = btcAmount / SATS;
  auto *amount = new QLineEdit();
  amount->setFixedWidth(CP_INPUT_WIDTH);
  amount->setText(QString::number(btcAmount, 'f', 1));
  amount->setEnabled(false);

  auto *btcLabel = new QLabel("BTC");

  auto *coinRow = (new qontrol::Row)
                      ->push(outpoint)
                      ->pushSpacer(10)
                      ->push(amount)
                      ->pushSpacer(10)
                      ->push(btcLabel)
                      ->pushSpacer();

  // Denomination
  auto *denomLabel = new QLabel("Denomination");
  denomLabel->setFixedWidth(CP_LABEL_WIDTH);

  double denomination =
      m_coin.value; // Value is updated in CreatePool::process();
  denomination = denomination / SATS;

  m_w_denomination = new QLineEdit();
  m_w_denomination->setFixedWidth(CP_INPUT_WIDTH);
  m_w_denomination->setValidator(m_denom_validator);
  m_w_denomination->setText(QString::number(denomination, 'f', 1));
  connect(m_w_denomination, &QLineEdit::editingFinished, this,
          [this]() { this->process(true); });

  auto *dbtcLabel = new QLabel("BTC");

  auto *denomRow = (new qontrol::Row)
                       ->pushSpacer(CP_L_SPACER)
                       ->push(denomLabel)
                       ->push(m_w_denomination)
                       ->pushSpacer(10)
                       ->push(dbtcLabel)
                       ->pushSpacer();

  // Fees
  auto *feesLabel = new QLabel("Fees");
  feesLabel->setFixedWidth(CP_LABEL_WIDTH);

  m_w_fee = new QLineEdit();
  m_w_fee->setFixedWidth(CP_INPUT_WIDTH);
  m_w_fee->setValidator(m_fees_validator);
  m_w_fee->setText(QString("1"));
  connect(m_w_fee, &QLineEdit::editingFinished, this,
          [this]() { this->process(); });

  auto *satLabel = new QLabel("sats/Vb");

  auto *feeRow = (new qontrol::Row)
                     ->pushSpacer(CP_L_SPACER)
                     ->push(feesLabel)
                     ->push(m_w_fee)
                     ->pushSpacer(10)
                     ->push(satLabel)
                     ->pushSpacer();

  // Peers
  auto *peersLabel = new QLabel("Peers");
  peersLabel->setFixedWidth(CP_LABEL_WIDTH);

  m_w_peers = new QLineEdit();
  m_w_peers->setFixedWidth(CP_INPUT_WIDTH);
  m_w_peers->setValidator(m_peers_validator);
  m_w_peers->setText("5");
  connect(m_w_peers, &QLineEdit::textEdited, this,
          [this]() { this->process(); });

  auto *peersRow = (new qontrol::Row)
                       ->pushSpacer(CP_L_SPACER)
                       ->push(peersLabel)
                       ->push(m_w_peers)
                       ->pushSpacer();

  // Timeout
  auto *timeoutLabel = new QLabel("Max duration");
  timeoutLabel->setFixedWidth(CP_LABEL_WIDTH);

  m_w_timeout = new QLineEdit();
  m_w_timeout->setFixedWidth(100);
  m_w_timeout->setValidator(m_timeout_validator);
  m_w_timeout->setText("24");
  connect(m_w_timeout, &QLineEdit::textEdited, this,
          [this]() { this->process(); });

  auto *hoursLabel = new QLabel("Hours");
  hoursLabel->setFixedWidth(120);

  auto *timeoutRow = (new qontrol::Row)
                         ->pushSpacer(CP_L_SPACER)
                         ->push(timeoutLabel)
                         ->push(m_w_timeout)
                         ->pushSpacer(10)
                         ->push(hoursLabel)
                         ->pushSpacer();

  // Buttons
  auto *createBtn = new QPushButton("Create");
  createBtn->setEnabled(false);
  m_create_btn = createBtn;
  connect(createBtn, &QPushButton::clicked, this, &CreatePool::onCreatePool);

  auto *cancelBtn = new QPushButton("Cancel");
  connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);

  auto *btnRow = (new qontrol::Row)
                     ->pushSpacer()
                     ->push(createBtn)
                     ->pushSpacer(100)
                     ->push(cancelBtn)
                     ->pushSpacer();

  auto *col = (new qontrol::Column)
                  ->push(titleRow)
                  ->pushSpacer(40)
                  ->push(coinRow)
                  ->pushSpacer(40)
                  ->push(denomRow)
                  ->pushSpacer(CP_V_SPACER)
                  ->push(feeRow)
                  ->pushSpacer(CP_V_SPACER)
                  ->push(peersRow)
                  ->pushSpacer(CP_V_SPACER)
                  ->push(timeoutRow)
                  ->pushSpacer()
                  ->push(btnRow)
                  ->pushSpacer(20);
  this->setMainWidget(col);
  this->process();
}

    auto CreatePool::fetch() -> bool {
        bool ok = true;

        double doubleDenom = m_w_denomination->text().toDouble(&ok);
        if (!ok) return false;
        m_denomination = std::floor(doubleDenom * SATS);

        m_fees = m_w_fee->text().toUInt(&ok);
        if (!ok) return false;

        m_peers = m_w_peers->text().toUInt(&ok);
        if (!ok) return false;

        m_max_duration = m_w_timeout->text().toUInt(&ok);
        if (!ok) return false;
        m_max_duration = m_max_duration * 60 * 60; // hours to seconds

        return true;
    }

    void CreatePool::process(bool force_denom) {
        if (!this->fetch()) {
            m_create_btn->setEnabled(false);
            return;
        }
    
        if (force_denom) {
            int maxDenomination = m_coin.value - VB_FEE;
            if (m_denomination > maxDenomination) {
                m_denomination = maxDenomination;
                m_fees = 1;
            } else {
                m_fees = std::floor((m_coin.value - m_denomination) / VB_FEE );
            }
        } else {
            m_fees = std::max<uint32_t>(m_fees, 1);
            auto fees = m_fees * VB_FEE;
            if (m_fees > m_coin.value ) {
                m_fees = m_coin.value;
                m_denomination = 0;
            } else {
                m_denomination = m_coin.value - fees;
            }
        }

        m_peers = std::max<size_t>(m_peers, 2);
        m_peers = std::min<size_t>(m_peers, 10);

        m_max_duration = std::min<uint64_t>(m_max_duration, 10 * 24 * 60 * 60); // 10 days
        m_max_duration = std::max<uint64_t>(m_max_duration, 1 * 60 * 60); // 1H

        auto denom = m_denomination / SATS;
        m_w_denomination->setText(QString::number(denom, 'f', 8)); // 8 decimal places
        m_w_fee->setText(QString::number(m_fees));
        m_w_peers->setText(QString::number(m_peers));
        m_w_timeout->setText(QString::number(m_max_duration / (60 * 60)));

        m_create_btn->setEnabled(true);
    }

    void CreatePool::onCreatePool() {
        this->process();

        connect(this, &CreatePool::createPool, m_controller, &AccountController::cmdCreatePool);
        connect(this, &CreatePool::createPool, this, [this]() {this->close();});

        emit this->createPool(
            m_coin.outpoint,
            m_denomination,
            m_fees,
            m_max_duration,
            m_peers
        );

    }

    } // namespace modal
