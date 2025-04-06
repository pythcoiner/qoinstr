#include "ComboLine.h"
#include "../screens/common.h"
#include "Row.h"
#include <qcombobox.h>

namespace widget {

ComboLine::ComboLine(const QString &label, const QStringList &values, QWidget *parent) {
    m_label = new QLabel(label);
    m_combo = new QComboBox();
    m_combo->addItems(values);
    this->push(m_label)
        ->pushSpacer(H_SPACER)
        ->push(m_combo)
        ->pushSpacer()
        ;
    this->setParent(parent);
}

auto ComboLine::label() -> QLabel* {
    return m_label;
}
auto ComboLine::combo() -> QComboBox* {
    return m_combo;
}

} // namespace widget
