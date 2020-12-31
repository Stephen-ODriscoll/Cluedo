#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Game.h"

#include "TakeTurn.h"
#include "PlayerInfo.h"
#include "Controller.h"

class Game : public QWidget
{
    Q_OBJECT

public:
    Game(Mode mode, int numPlayers, QWidget* parent = Q_NULLPTR);
    ~Game();

    void refresh();

    void critical(const str& title, const str& desc);
    const fs::path openCluedoTextFile(const str& issue);

private:
    size_t stageDisplayed;

    Ui::GameObject ui;
    Controller controller;
    QWidget* pPopUp;
    bool hide;

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
