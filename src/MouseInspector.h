#pragma once

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>

class MouseInspector : public QObject {
    Q_OBJECT
public:
    explicit MouseInspector(QObject *parent = nullptr) : QObject(parent) {
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
