#include "stdafx.h"
#include "TakeTurn.h"

TakeTurn::TakeTurn(Game* pGame, std::shared_ptr<const Turn> pOldTurn, QWidget* parent) :
    TakeTurn(pGame, pOldTurn->pDetective->name, pOldTurn->witness(), parent)  // Call other constructor (I ain't doing everything twice)
{
    this->pOldTurn = pOldTurn;

    switch (pOldTurn->action)
    {
    case Action::MISSED:
        missedButtonClicked();
        break;

    case Action::ASKED:
    {
        askedButtonClicked();

        std::shared_ptr<const Asked> pOldAsked = std::static_pointer_cast<const Asked>(pOldTurn);

        auto it1 = categoryBoxes.begin();
        for (auto it2 = pOldAsked->pCards.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
            (*it1)->setCurrentIndex((*it1)->findText((*it2)->name.c_str()));

        if (pOldAsked->shown)
        {
            if (pOldAsked->cardShown.empty())
                outcomeTrueClicked();
            else if (pOldAsked->cardShown.c_str() == ui.cat1Box->currentText())
                cat1ShownClicked();
            else if (pOldAsked->cardShown.c_str() == ui.cat2Box->currentText())
                cat2ShownClicked();
            else if (pOldAsked->cardShown.c_str() == ui.cat3Box->currentText())
                cat3ShownClicked();
            else
            {
                // Should never happen
                QMessageBox msgBox;
                msgBox.critical(0, "Error", "Failed to deduce card chosen in previous turn");
            }
        }
        else
            outcomeFalseClicked();

        break;
    }

    case Action::GUESSED:
        guessedButtonClicked();

        std::shared_ptr<const Guessed> pOldGuessed = std::static_pointer_cast<const Guessed>(pOldTurn);

        auto it1 = categoryBoxes.begin();
        for (auto it2 = pOldGuessed->pCards.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
            (*it1)->setCurrentIndex((*it1)->findText((*it2)->name.c_str()));

        if (pOldGuessed->correct)
            outcomeTrueClicked();
        else
            outcomeFalseClicked();

        break;
    }
}

TakeTurn::TakeTurn(Game* pGame, const str& detective, const str& probableWitness, QWidget* parent) :
    pGame(pGame),
    detective(detective),
    pOldTurn(nullptr),
    pPopUp(nullptr),
    QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle((detective + str("'s Turn")).c_str());

    // Player stuff
    {
        ui.askedBox->setEnabled(false);

        // Add players for who is being asked
        for (const Player& player : g_players)
            if (player.name != detective)
                ui.askedBox->addItem(player.name.c_str());

        // We make a guess that the person being asked is probably the person whose turn is next but this can be changed
        // If we're editing a previous turn and don't have a probable witness use the first player that appears
        ui.askedBox->setCurrentText(probableWitness.empty() ? ui.askedBox->itemText(0) : probableWitness.c_str());
    }

    // Cards stuff
    {
        categoryBoxes = { ui.cat1Box, ui.cat2Box, ui.cat3Box };
        assert(categoryBoxes.size() == NUM_CATEGORIES);

        // Adds the cards in each category to each combo box
        auto it1 = categoryBoxes.begin();
        for (auto it2 = g_categories.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
        {
            (*it1)->setEnabled(false);  // Set the combo box as disabled until action is chosen

            // Add cards for this category
            for (auto& item : it2->cards)
                (*it1)->addItem(item.name.c_str());
        }
    }

    // Hide check boxes until user selects an action
    {
        ui.catShownWidget->setVisible(false);
        ui.outcomeTrue->setVisible(false);
        ui.outcomeFalse->setVisible(false);
    }

    ui.submitButton->setEnabled(false);

    connect(ui.missedButton, SIGNAL(clicked()), this, SLOT(missedButtonClicked()));
    connect(ui.askedButton, SIGNAL(clicked()), this, SLOT(askedButtonClicked()));
    connect(ui.guessedButton, SIGNAL(clicked()), this, SLOT(guessedButtonClicked()));

    connect(ui.cat1Shown, SIGNAL(clicked()), this, SLOT(cat1ShownClicked()));
    connect(ui.cat2Shown, SIGNAL(clicked()), this, SLOT(cat2ShownClicked()));
    connect(ui.cat3Shown, SIGNAL(clicked()), this, SLOT(cat3ShownClicked()));
    connect(ui.outcomeTrue, SIGNAL(clicked()), this, SLOT(outcomeTrueClicked()));
    connect(ui.outcomeFalse, SIGNAL(clicked()), this, SLOT(outcomeFalseClicked()));

    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    connect(ui.submitButton, SIGNAL(clicked()), this, SLOT(submitButtonClicked()));
}

TakeTurn::~TakeTurn()
{
    delete pPopUp;
}

bool TakeTurn::outcomeChosen()
{
    return ui.outcomeTrue->isChecked() || ui.outcomeFalse->isChecked()
        || (ui.askedButton->isDefault() && (ui.cat1Shown->isChecked() || ui.cat2Shown->isChecked() || ui.cat3Shown->isChecked()));
}

std::shared_ptr<Turn> TakeTurn::getNewTurn()
{
    const auto itDetective = std::find(g_players.begin(), g_players.end(), detective);
    if (itDetective == g_players.end())
        throw std::exception((str("Failed to find detective player ") + detective).c_str());


    // Missed a turn
    if (ui.missedButton->isDefault())
        return std::make_shared<Missed>(&*itDetective);


    std::vector<Card*> pCards;
    auto it1 = categoryBoxes.begin();
    for (auto it2 = g_categories.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
    {
        const str card = (*it1)->currentText().toStdString();
        auto itCard = std::find(it2->cards.begin(), it2->cards.end(), card);
        if (itCard == it2->cards.end())
            throw std::exception((str("Failed to find selected card ") + card).c_str());

        pCards.push_back(&*itCard);
    }


    // Asked a question
    if (ui.askedButton->isDefault())
    {
        const str witness = ui.askedBox->currentText().toStdString();
        const auto itWitness = std::find(g_players.begin(), g_players.end(), witness);
        if (itWitness == g_players.end())
            throw std::exception((str("Failed to find witness player ") + witness).c_str());

        if (ui.outcomeTrue->isChecked())
            return std::make_shared<Asked>(&*itDetective, &*itWitness, pCards, true);
        else if (ui.outcomeFalse->isChecked())
            return std::make_shared<Asked>(&*itDetective, &*itWitness, pCards, false);
        else if (ui.cat1Shown->isChecked())
            return std::make_shared<Asked>(&*itDetective, &*itWitness, pCards, true, ui.cat1Box->currentText().toStdString());
        else if (ui.cat2Shown->isChecked())
            return std::make_shared<Asked>(&*itDetective, &*itWitness, pCards, true, ui.cat2Box->currentText().toStdString());
        else if (ui.cat3Shown->isChecked())
            return std::make_shared<Asked>(&*itDetective, &*itWitness, pCards, true, ui.cat3Box->currentText().toStdString());
        else
            throw std::exception("Failed to deduce chosen outcome");
    }


    // Made a guess
    if (ui.guessedButton->isDefault())
    {
        if (ui.outcomeTrue->isChecked())
            return std::make_shared<Guessed>(&*itDetective, pCards, true);
        else if (ui.outcomeFalse->isChecked())
            return std::make_shared<Guessed>(&*itDetective, pCards, false);
        else
            throw std::exception("Failed to deduce guessed outcome");
    }

    throw std::exception("Failed to deduce chosen action");
}

void TakeTurn::closeEvent(QCloseEvent* event)
{
    if (pPopUp)
        pPopUp->close();

    event->accept();
}

void TakeTurn::missedButtonClicked()
{
    ui.missedButton->setDefault(true);
    ui.askedButton->setDefault(false);
    ui.guessedButton->setDefault(false);

    ui.askedBox->setEnabled(false);

    for (QComboBox* catBox : categoryBoxes)
        catBox->setEnabled(false);

    ui.catShownWidget->setVisible(false);
    ui.outcomeTrue->setVisible(false);
    ui.outcomeFalse->setVisible(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::askedButtonClicked()
{
    ui.missedButton->setDefault(false);
    ui.askedButton->setDefault(true);
    ui.guessedButton->setDefault(false);

    ui.askedBox->setEnabled(true);

    for (QComboBox* catBox : categoryBoxes)
        catBox->setEnabled(true);

    ui.catShownWidget->setVisible(true);
    ui.outcomeTrue->setText("  Don't know which was shown");
    ui.outcomeTrue->setVisible(true);
    ui.outcomeFalse->setText("  Nothing was shown");
    ui.outcomeFalse->setVisible(true);

    ui.submitButton->setEnabled(outcomeChosen());
}

void TakeTurn::guessedButtonClicked()
{
    ui.missedButton->setDefault(false);
    ui.askedButton->setDefault(false);
    ui.guessedButton->setDefault(true);

    ui.askedBox->setEnabled(false);

    for (QComboBox* catBox : categoryBoxes)
        catBox->setEnabled(true);

    ui.catShownWidget->setVisible(false);
    ui.outcomeTrue->setText("  Guessed correct");
    ui.outcomeTrue->setVisible(true);
    ui.outcomeFalse->setText("  Guessed wrong");
    ui.outcomeFalse->setVisible(true);

    ui.submitButton->setEnabled(outcomeChosen());
}

void TakeTurn::cat1ShownClicked()
{
    ui.cat1Shown->setChecked(true);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cat2ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(true);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cat3ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(true);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::outcomeTrueClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(true);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::outcomeFalseClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(true);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cancelButtonClicked()
{
    close();
}

void TakeTurn::submitButtonClicked()
{
    std::shared_ptr<Turn> pNewTurn = getNewTurn();

    if (pNewTurn->shouldRedistribute())
    {
        delete pPopUp;
        pPopUp = new RedistributeCards(pGame, std::static_pointer_cast<Guessed>(pNewTurn), pOldTurn);
        pPopUp->show();
    }
    else
    {
        TRY
            pGame->controller.processTurn(pNewTurn, pOldTurn);
            close();
        CATCH
            
        pGame->refresh();
    }
}
