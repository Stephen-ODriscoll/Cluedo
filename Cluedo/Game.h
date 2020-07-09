#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Game.h"

#include "TakeTurn.h"
#include "Controller.h"

class TakeTurn;
class Game : public QWidget
{
    Q_OBJECT

public:
    Game(Controller* pController, QWidget* parent = Q_NULLPTR);
    ~Game();

    void startGame();
    void updateStatus();
    void rotateTurn();

private:
    Ui::GameObject ui;
    Controller* pController;
    TakeTurn* pTakeTurn;

private slots:
    void upButtonClicked();
    void downButtonClicked();
    void renameButtonClicked();
    void turnButtonClicked();
    void editTurnButtonClicked();
};
