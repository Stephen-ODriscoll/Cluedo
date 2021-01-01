#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Game.h"

#include "Controller.h"

class Game : public QWidget
{
    Q_OBJECT
        
    bool hide;
    size_t stageDisplayed;

    Ui::GameObject ui;
    QWidget* pPopUp;


public:
    Controller controller;

    Game(Mode mode, int numPlayers, QWidget* parent = Q_NULLPTR);
    ~Game();

    void refresh();
    const fs::path openCluedoTextFile(const str& issue);

    
private slots:
    void upButtonClicked();
    void downButtonClicked();
    void playerInfoButtonClicked();
    void turnButtonClicked();
    void editTurnButtonClicked();
    void playersListRowChanged(int row);
    void stageBoxChanged(const QString& text);
    void hideBoxStageChanged(int state);
};

#include "TakeTurn.h"
#include "PlayerInfo.h"
