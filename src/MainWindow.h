#pragma once

#include <Qontrol>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

class MainWindow : public Window {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void loadLayout(QLayout *layout);
    auto centralWidget() -> QWidget*;

    void loadPanel(QWidget *widget) override;
    auto takePanel() -> QWidget* override;

signals:

private:
    Column *m_side_menu = nullptr;
    QWidget *m_panel_widget = nullptr;

    void initWindow();
};
