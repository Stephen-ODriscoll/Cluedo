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
    PlayerInfo(Controller* pController, Game* pGame, const Player* pPlayer, QWidget* parent = Q_NULLPTR);

private:
    Ui::PlayerInfoObject ui;
    Controller* pController;
    Game* pGame;
    const Player* pPlayer;

    void updateInfo();
    void setCardButtonText(const str& cardName, QPushButton* pButton);
    void addRemoveCard(const str& cardName, QPushButton* pButton);

private slots:
    void renameButtonClicked();
    bool eventFilter(QObject* watched, QEvent* event);
    void cat1BoxChanged(const QString& text);
    void cat2BoxChanged(const QString& text);
    void cat3BoxChanged(const QString& text);
    void cat1ButtonClicked();
    void cat2ButtonClicked();
    void cat3ButtonClicked();
};