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
    Game(Controller* pController, QWidget* parent = Q_NULLPTR);
    ~Game();

    void startGame();
    void updateNotes();
    void moveToBack(const str& playerName);
    void removePlayer(const str& playerName);
    void editName(const str& oldName, const str& newName);

private:
    Ui::GameObject ui;
    Controller* pController;
    QWidget* pPopUp;

    int findItem(const str& playerName);

private slots:
    void upButtonClicked();
    void downButtonClicked();
    void playerInfoButtonClicked();
    void turnButtonClicked();
    void editTurnButtonClicked();
};
