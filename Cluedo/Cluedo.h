#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Cluedo.h"

#include "Controller.h"

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

    std::wstring openCluedoTextFile(const str& issue);

    void showGame();
    void critical(const str& title, const str& desc);
    Game* game();

private:
    Ui::CluedoObject ui;

    Controller* pController;
    Home* pHome;
    Game* pGame;
};
