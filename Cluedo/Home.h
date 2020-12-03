#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Home.h"

#include "Controller.h"

class Cluedo;
class Home : public QWidget
{
    Q_OBJECT

public:
    Home(Cluedo* pCluedo, QWidget* parent = Q_NULLPTR);

private:
    Ui::HomeObject ui;
    Cluedo* pCluedo;

private slots:
    void singleModeButtonClicked();
    void groupModeButtonClicked();
    void continueButtonClicked();
};

#include "Cluedo.h"
