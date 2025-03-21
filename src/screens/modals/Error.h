#pragma once

#include <qdialog.h>
#include <qtmetamacros.h>

namespace modal {

class Error : public QDialog {
    Q_OBJECT
public:
    Error(const QString& msg);
    Error(const QString& msg, const QString& title);
};

} // namespace modal
