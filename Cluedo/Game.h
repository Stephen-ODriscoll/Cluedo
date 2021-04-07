#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Game.h"

#include "Controller.h"

class Game : public QWidget
{
    Q_OBJECT

    bool hide = false;
    bool isRefreshing = false;
    size_t stageIndex = 0;

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
    void playersListRowChanged(const int row);
    void stageBoxChanged(const int index);
    void hideBoxStageChanged();
};

#include "TakeTurn.h"
#include "PlayerInfo.h"
