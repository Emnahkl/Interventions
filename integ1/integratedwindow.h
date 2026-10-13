#ifndef INTEGRATEDWINDOW_H
#define INTEGRATEDWINDOW_H

#include <QMainWindow>

class QStackedWidget;

class IntegratedWindow : public QMainWindow
{
public:
    explicit IntegratedWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *pages;
};

#endif

