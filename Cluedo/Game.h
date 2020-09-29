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

    void updateNotes();
    void moveToBack(const str& playerName);
    void removePlayerAndAddStage(const str& playerName);
    void editName(const str& oldName, const str& newName);

    void critical(const str& title, const str& desc);
    std::wstring openCluedoTextFile(const str& issue);

private:
    size_t stageDisplayed;

    Ui::GameObject ui;
    Controller controller;
    QWidget* pPopUp;

    size_t playersLeft();
    int findPlayerIndex(const str& playerName);

private slots:
    void upButtonClicked();
    void downButtonClicked();
    void playerInfoButtonClicked();
    void turnButtonClicked();
    void editTurnButtonClicked();
    void stageBoxChanged(const QString& text);
};
