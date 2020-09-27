#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlayerInfo.h"

#include "Game.h"
#include "Controller.h"

class Game;
class PlayerInfo : public QWidget
{
    Q_OBJECT

public:
    PlayerInfo(Controller* pController, Game* pGame, const Player* pPlayer, const size_t stageNum, QWidget* parent = Q_NULLPTR);

private:
    Ui::PlayerInfoObject ui;
    Controller* pController;
    Game* pGame;

    const Player* pPlayer;
    size_t stageDisplayed;
    std::vector<str> cardNames;

    void updateInfo();
    void toggleCardOwned(const str& cardName);

private slots:
    void stageBoxChanged(const QString& text);
    void cat1BoxChanged(const QString& text);
    void cat2BoxChanged(const QString& text);
    void cat3BoxChanged(const QString& text);
    void resetButtonClicked();
    void applyButtonClicked();
};