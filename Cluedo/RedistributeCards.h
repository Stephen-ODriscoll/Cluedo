#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RedistributeCards.h"

#include "Controller.h"

class RedistributeCards : public QWidget
{
    Q_OBJECT

public:
    RedistributeCards(Controller* pController, std::shared_ptr<Guessed> pGuessed, std::shared_ptr<const Turn> pOldTurn, QWidget* parent = Q_NULLPTR);

private:
    Ui::RedistributeCardsObject ui;
    Controller* pController;

    std::shared_ptr<Guessed> pGuessed;
    std::shared_ptr<const Turn> pOldTurn;
    std::vector<std::pair<QTextEdit*, QSpinBox*>> pairs;

private slots:
    void cancelButtonClicked();
    void okayButtonClicked();
};
