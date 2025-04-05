#pragma once

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QApplication>
#include <QCursor>
#include <QDebug>

class MouseInspector : public QObject {
    Q_OBJECT
public:
    explicit MouseInspector(QObject *parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
