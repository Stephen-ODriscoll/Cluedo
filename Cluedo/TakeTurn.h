#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TakeTurn.h"

#include "RedistributeCards.h"
#include "Controller.h"

class TakeTurn : public QWidget
{
    Q_OBJECT

public:
    TakeTurn(Controller* pController, std::shared_ptr<const Turn> pOldTurn, QWidget* parent = Q_NULLPTR);
    TakeTurn(Controller* pController, const str& detective, const str& probableWitness, QWidget* parent = Q_NULLPTR);
    ~TakeTurn();

private:
    Ui::TakeTurnObject ui;
    Controller* pController;

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