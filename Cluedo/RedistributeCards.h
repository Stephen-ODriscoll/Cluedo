#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RedistributeCards.h"

#include "Game.h"

class RedistributeCards : public QWidget
{
    Q_OBJECT

public:
    RedistributeCards(Game* pGame, std::shared_ptr<Guessed> pGuessed, std::shared_ptr<const Turn> pOldTurn, QWidget* parent = Q_NULLPTR);

private:
    Game* pGame;
    Ui::RedistributeCardsObject ui;
    
    std::shared_ptr<Guessed> pGuessed;
    std::shared_ptr<const Turn> pOldTurn;
    std::vector<std::pair<QTextEdit*, QSpinBox*>> pairs;

private slots:
    void cancelButtonClicked();
    void okayButtonClicked();
};
