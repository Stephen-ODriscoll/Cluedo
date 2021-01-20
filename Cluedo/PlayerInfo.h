#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlayerInfo.h"

#include "Game.h"

class PlayerInfo : public QWidget
{
    Q_OBJECT

public:
    PlayerInfo(Game* pGame, const Player* pPlayer, const size_t stageNum, QWidget* parent = Q_NULLPTR);

private:
    Game* pGame;
    Ui::PlayerInfoObject ui;

    const Player* pPlayer;
    size_t stageIndex;
    std::vector<StagePreset> presets;

    void updateInfo();
    void setButtonText(Card& card, QPushButton* button);
    void toggleCardOwned(Card& card, QPushButton* button);

private slots:
    void stageBoxChanged(const int index);
    void numCardsBoxChanged(const int value);
    void cat1BoxChanged(const int index);
    void cat2BoxChanged(const int index);
    void cat3BoxChanged(const int index);
    void cat1ButtonClicked();
    void cat2ButtonClicked();
    void cat3ButtonClicked();
    void resetButtonClicked();
    void applyButtonClicked();
    bool eventFilter(QObject* object, QEvent* event);
};