#pragma once

#include "Row.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace widget {

class ComboLine : public qontrol::Row {
    Q_OBJECT
public:
    ComboLine(const QString &label, const QStringList &values, QWidget *parent = nullptr);
    auto label() -> QLabel*;
    auto combo() -> QComboBox*;
private:
    QLabel *m_label = nullptr;
    QComboBox *m_combo = nullptr;
};

} // namespace widget
