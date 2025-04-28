#pragma once

#include "Modal.h"
#include "screens/Send.h"
#include <optional>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qtmetamacros.h>

namespace modal {

class SelectCoins;

class CoinWidget : public QObject {
    Q_OBJECT
public:
    CoinWidget(const screen::Coin &coin, SelectCoins *modal);
    auto isChecked() -> bool;
    auto setCheckable(bool checkable);
    auto coin() -> screen::Coin;
    auto checkbox() -> QCheckBox *;
    auto outpoint() -> QLineEdit *;
    auto label() -> QLineEdit *;
    auto amount() -> QLineEdit *;

public slots:
    void updateLabel();

private:
    screen::Coin m_coin;
    QCheckBox *m_checkbox = nullptr;
    QLineEdit *m_outpoint = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_value = nullptr;
};

class SelectCoins : public qontrol::Modal {
    Q_OBJECT
public:
    SelectCoins(const QList<screen::Coin> &coins);
    SelectCoins(const QList<screen::Coin> &coins, const QString &relay_url);

    void init(const QList<screen::Coin> &coins);
    void view();
    auto filter(const QList<CoinWidget *> &coins) -> QList<CoinWidget *>;
    auto sort(const QList<CoinWidget *> &coins) -> QList<CoinWidget *>;
    auto getCoins() -> QList<CoinWidget *>;
    void applyFilter();

signals:
    void coinsSelected(QList<screen::Coin> coins);
    void coinSelectedForPool(screen::Coin coin, QString relay_url);

public slots:
    void checked();
    void onAbort();
    void onOk();

private:
    std::optional<QString> m_relay_url;
    int m_amount_width = 150;
    int m_label_width = 200;
    int m_outpoint_width = 200;
    QWidget *m_widget = nullptr;
    QLineEdit *m_label_filter = nullptr;
    QLineEdit *m_total = nullptr;
    QLabel *m_total_label = nullptr;
    QPushButton *m_value_up = nullptr;
    QPushButton *m_value_down = nullptr;
    QButtonGroup *m_value_group = nullptr;
    QPushButton *m_abort = nullptr;
    QPushButton *m_ok = nullptr;
    QHash<int, CoinWidget *> m_coins;
};

} // namespace modal
