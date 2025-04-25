#pragma once

#include "Column.h"
#include "Modal.h"
#include "Row.h"
#include "common.h"
#include "screens/common.h"
#include <cstdint>
#include <qcheckbox.h>
#include <qhash.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qtmetamacros.h>

namespace modal {

struct Coin {
    QString outpoint;
    QString label;
    uint64_t value{};
};

class CoinWidget : public QObject {
    Q_OBJECT
public:
    CoinWidget(const Coin &coin);
    auto isChecked() -> bool;
    auto coin() -> Coin;
    auto checkbox() -> QCheckBox *;
    auto outpoint() -> QLineEdit *;
    auto label() -> QLineEdit *;
    auto amount() -> QLineEdit *;

public slots:
    void updateLabel();

private:
    Coin m_coin;
    QCheckBox *m_checkbox = nullptr;
    QLineEdit *m_outpoint = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_value = nullptr;
};

class SelectCoins : public qontrol::Modal {
    Q_OBJECT
public:
    SelectCoins(const QList<Coin> &coins) {
        init(coins);
        view();
    }

    void init(const QList<Coin> &coins);
    void view();

    auto filter(const QList<CoinWidget *> &coins) -> QList<CoinWidget *> {
        return coins;
    }

    auto sort(const QList<CoinWidget *> &coins) -> QList<CoinWidget *> {
        return coins;
    }

    auto getCoins() -> QList<CoinWidget *>;

signals:
    void coinsSelected(QList<Coin> coins);

public slots:
    void onAbort();
    void onOk();

private:
    int m_amount_width = 150;
    int m_label_width = 200;
    int m_outpoint_width = 200;
    QWidget *m_widget = nullptr;
    QLineEdit *m_label_filter = nullptr;
    QPushButton *m_value_up = nullptr;
    QPushButton *m_value_down = nullptr;
    QPushButton *m_abort = nullptr;
    QPushButton *m_ok = nullptr;
    QHash<int, CoinWidget *> m_coins;
};

} // namespace modal
