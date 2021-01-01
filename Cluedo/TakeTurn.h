#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TakeTurn.h"

#include "Game.h"
#include "RedistributeCards.h"

class TakeTurn : public QWidget
{
    Q_OBJECT

public:
    TakeTurn(Game* pGame, std::shared_ptr<const Turn> pOldTurn, QWidget* parent = Q_NULLPTR);
    TakeTurn(Game* pGame, const str& detective, const str& probableWitness, QWidget* parent = Q_NULLPTR);
    ~TakeTurn();

private:
    Game* pGame;
    Ui::TakeTurnObject ui;

    QWidget* pPopUp;

    const str detective;
    std::shared_ptr<const Turn> pOldTurn;
    std::vector<QComboBox*> categoryBoxes;

    size_t nextId();
    bool outcomeChosen();
    std::shared_ptr<Turn> getNewTurn();

private slots:
    void missedButtonClicked();
    void askedButtonClicked();
    void guessedButtonClicked();

    void cat1ShownClicked();
    void cat2ShownClicked();
    void cat3ShownClicked();
    void outcomeTrueClicked();
    void outcomeFalseClicked();

    void cancelButtonClicked();
    void submitButtonClicked();
};