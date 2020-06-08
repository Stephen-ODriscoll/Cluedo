#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Game.h"
#include "Controller.h"

class Game : public QWidget
{
    Q_OBJECT

public:
    Game(Controller* pController, QWidget* parent = Q_NULLPTR);

    void startGame();

private:
    Ui::GameObject ui;
    Controller* pController;

private slots:
    void upButtonClicked();
    void downButtonClicked();
    void renameButtonClicked();
    void turnButtonClicked();
};
