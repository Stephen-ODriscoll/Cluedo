#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Cluedo.h"

#include "Home.h"
#include "Game.h"

class Home;
class Game;
class Cluedo : public QMainWindow
{
    Q_OBJECT

public:
    Cluedo(QWidget *parent = Q_NULLPTR);
    ~Cluedo();

    void startGame(Mode mode, int numPlayers);
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::CluedoObject ui;

    Home* pHome;
    Game* pGame;
};
