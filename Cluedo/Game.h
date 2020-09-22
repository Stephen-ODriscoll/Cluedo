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
    void removePlayerAndAddStage(const str& playerName);
    void editName(const str& oldName, const str& newName);

private:
    size_t stageDisplayed;

    Ui::GameObject ui;
    Controller* pController;
    QWidget* pPopUp;

    int findItemIndex(const str& playerName);

private slots:
    void upButtonClicked();
    void downButtonClicked();
    void playerInfoButtonClicked();
    void turnButtonClicked();
    void editTurnButtonClicked();
    void stageBoxChanged(const QString& text);
};
