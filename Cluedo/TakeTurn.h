#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TakeTurn.h"
#include "Controller.h"

class TakeTurn : public QWidget
{
    Q_OBJECT

public:
    TakeTurn(Controller* pController, const str& detective, const str& probableWitness, QWidget* parent = Q_NULLPTR);

private:
    Ui::TakeTurnObject ui;
    Controller* pController;

    const str detective;
    std::vector<QComboBox*> categoryBoxes;

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