#pragma once

#include "Modal.h"
#include "include/cpp_joinstr.h"
#include <optional>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qtmetamacros.h>

namespace modal {

class SelectCoins;

class CoinWidget : public QObject {
    Q_OBJECT
public:
    CoinWidget(const RustCoin &coin, SelectCoins *modal);
    auto isChecked() -> bool;
    auto setCheckable(bool checkable);
    void setChecked(bool checked);
    auto coin() -> RustCoin;
    auto checkbox() -> QCheckBox *;
    auto outpoint() -> QLineEdit *;
    auto label() -> QLineEdit *;
    auto amount() -> QLineEdit *;

public slots:
    void updateLabel();

private:
    RustCoin m_coin;
    QCheckBox *m_checkbox = nullptr;
    QLineEdit *m_outpoint = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_value = nullptr;
};

class SelectCoins : public qontrol::Modal {
    Q_OBJECT
public:
    SelectCoins(const QList<RustCoin> &coins);
    SelectCoins(const QList<RustCoin> &coins, const QString &relay_url);
    SelectCoins(const QList<RustCoin> &coins, const QString &relay_url,
                const QString &pool_id);

    void init(const QList<RustCoin> &coins);
    void view();
    auto filter(const QList<CoinWidget *> &coins) -> QList<CoinWidget *>;
    auto sort(const QList<CoinWidget *> &coins) -> QList<CoinWidget *>;
    auto getCoins() -> QList<CoinWidget *>;
    void applyFilter();

signals:
    void coinsSelected(QList<RustCoin> coins);
    void coinSelectedForCreatePool(RustCoin coin, QString relay_url);
    void coinSelectedForJoinPool(RustCoin coin, QString pool_id,
                                 QString relay_url);

public slots:
    void checked();
    void onAbort();
    void onOk();

private:
    std::optional<QString> m_relay_url;
    std::optional<QString> m_pool_id;

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
