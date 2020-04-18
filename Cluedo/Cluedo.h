#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Cluedo.h"

class Cluedo : public QMainWindow
{
    Q_OBJECT

public:
    Cluedo(QWidget *parent = Q_NULLPTR);

private:
    Ui::CluedoClass ui;

private slots:
    void singleModeButtonClicked();
    void groupModeButtonClicked();
};
