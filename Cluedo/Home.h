#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Home.h"
#include "Controller.h"

class Home : public QWidget
{
    Q_OBJECT

public:
    Home(Controller* pController, QWidget* parent = Q_NULLPTR);

private:
    Ui::HomeObject ui;
    Controller* pController;

private slots:
    void singleModeButtonClicked();
    void groupModeButtonClicked();
    void continueButtonClicked();
};
