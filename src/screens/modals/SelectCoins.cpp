#include "SelectCoins.h"
#include "Column.h"
#include "Row.h"
#include "screens/common.h"
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qtimer.h>

namespace modal {

void CoinWidget::updateLabel() {
    // TODO: update label on backend
    m_coin.label = m_label->text();
}

auto CoinWidget::isChecked() -> bool {
    return m_checkbox->isChecked();
}

auto CoinWidget::coin() -> Coin {
    return m_coin;
}

void SelectCoins::onOk() {
    auto coins = QList<Coin>();
    for (auto id : m_coins.keys()) {
        auto *cw = m_coins.value(id);
        if (cw->isChecked()) {
            coins.append(cw->coin());
        }
    }
    emit coinsSelected(coins);
    close();
}

void SelectCoins::onAbort() {
    close();
}

void SelectCoins::init(const QList<Coin> &coins) {
    setWindowTitle("Select coin(s)");
    setFixedSize(700, 400);

    m_label_filter = new QLineEdit();
    m_label_filter->setPlaceholderText("label filter...");

    m_value_up = new QPushButton();
    auto up = m_value_up->style()->standardIcon(QStyle::SP_ArrowUp);
    m_value_up->setIcon(up);
    m_value_up->setFixedWidth(60);
    m_value_up->setCheckable(true);

    m_value_down = new QPushButton();
    auto down = m_value_down->style()->standardIcon(QStyle::SP_ArrowDown);
    m_value_down->setIcon(down);
    m_value_down->setFixedWidth(60);
    m_value_down->setCheckable(true);

    connect(m_value_up, &QPushButton::toggled, [this]() {
        if (this->m_value_up->isChecked()) {
            this->m_value_down->setChecked(false);
        }
        this->view();
    });
    connect(m_value_down, &QPushButton::toggled, [this]() {
        if (this->m_value_down->isChecked()) {
            this->m_value_up->setChecked(false);
        }
        this->view();
    });

    m_abort = new QPushButton("Cancel");
    m_abort->setFixedWidth(150);

    m_ok = new QPushButton("Ok");
    m_ok->setFixedWidth(150);

    int id = 0;
    for (const auto &coin : coins) {
        auto *cw = new CoinWidget(coin);
        m_coins.insert(id, cw);
        id++;
    }
}

void SelectCoins::view() {
    auto filtered = filter(getCoins());
    auto sorted = sort(filtered);

    auto *oldWidget = m_widget;

    int arrowBtnWidth = m_value_up->sizeHint().height();
    m_value_up->setFixedWidth(arrowBtnWidth);
    m_value_down->setFixedWidth(arrowBtnWidth);
    m_label_filter->setFixedWidth(m_label_width);
    int spacer = 25 + m_outpoint_width + H_SPACER;

    int valueSpacer = m_amount_width - (2 * arrowBtnWidth);
    auto *firstRow = (new qontrol::Row)
                         ->pushSpacer(spacer)
                         ->push(m_label_filter)
                         ->pushSpacer(H_SPACER)
                         ->push(m_value_up)
                         ->pushSpacer(valueSpacer)
                         ->push(m_value_down)
                         ->pushSpacer();

    auto *coinsCol = new qontrol::Column;

    for (auto *cw : sorted) {
        cw->outpoint()->setFixedWidth(200);
        cw->label()->setFixedWidth(200);
        cw->amount()->setFixedWidth(m_amount_width);
        auto *row = (new qontrol::Row)
                        ->push(cw->checkbox())
                        ->pushSpacer(H_SPACER)
                        ->push(cw->outpoint())
                        ->pushSpacer(H_SPACER)
                        ->push(cw->label())
                        ->pushSpacer(H_SPACER)
                        ->push(cw->amount())
                        ->pushSpacer();
        coinsCol->pushSpacer(V_SPACER)->push(row);
    }
    auto *scroll = new QScrollArea;
    scroll->setWidget(coinsCol);
    scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    auto *lastRow = (new qontrol::Row)
                        ->pushSpacer()
                        ->push(m_abort)
                        ->pushSpacer()
                        ->push(m_ok)
                        ->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->push(firstRow)
                    ->pushSpacer(10)
                    ->push(scroll)
                    ->pushSpacer(20)
                    ->push(lastRow)
                    ->pushSpacer();

    m_widget = margin(col);

    delete oldWidget;
    setMainWidget(m_widget);
}

auto SelectCoins::getCoins() -> QList<CoinWidget *> {
    auto coins = QList<CoinWidget *>();
    for (auto *coin : m_coins) {
        coins.append(coin);
    }
    return coins;
}

auto CoinWidget::checkbox() -> QCheckBox * {
    return m_checkbox;
}

auto CoinWidget::label() -> QLineEdit * {
    return m_label;
}

auto CoinWidget::amount() -> QLineEdit * {
    return m_value;
}

auto CoinWidget::outpoint() -> QLineEdit * {
    return m_outpoint;
}

CoinWidget::CoinWidget(const Coin &coin) {
    m_coin = coin;
    m_checkbox = new QCheckBox();

    m_outpoint = new QLineEdit();
    m_outpoint->setText(m_coin.outpoint);
    m_outpoint->setEnabled(false);

    m_label = new QLineEdit();
    m_label->setText(m_coin.label);
    connect(m_label, &QLineEdit::textEdited, this, &CoinWidget::updateLabel,
            qontrol::UNIQUE);

    m_value = new QLineEdit();
    double fValue = m_coin.value;
    fValue /= SATS;
    auto btcValue = QString::number(fValue) + " BTC";
    m_value->setText(btcValue);
    m_value->setEnabled(false);
}

auto SelectCoins::sort(const QList<CoinWidget *> &coins)
    -> QList<CoinWidget *> {
    QList<CoinWidget *> sortedCoins = coins;

    bool sortAscendingAmount = m_value_up->isChecked();
    bool sortDescendingAmount = m_value_down->isChecked();

    if (sortAscendingAmount) {
        std::ranges::sort(sortedCoins, [](CoinWidget *a, CoinWidget *b) {
            return a->coin().value < b->coin().value;
        });
    } else if (sortDescendingAmount) {
        std::ranges::sort(sortedCoins, [](CoinWidget *a, CoinWidget *b) {
            return a->coin().value > b->coin().value;
        });
    } else {
        std::ranges::sort(sortedCoins, [](CoinWidget *a, CoinWidget *b) {
            return a->coin().outpoint < b->coin().outpoint;
        });
    }
    return sortedCoins;
}
} // namespace modal
