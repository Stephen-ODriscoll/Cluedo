#include "stdafx.h"
#include "TakeTurn.h"

TakeTurn::TakeTurn(Controller* pController, const str& detective, const str& probableWitness, QWidget* parent) :
    pController(pController),
    detective(detective),
    QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle((detective + str("'s Turn")).c_str());

    // Player stuff
    {
        ui.askedBox->setEnabled(false);

        // Add players for who is being asked
        for (const Player& player : pController->players())
            if (player.name != detective)
                ui.askedBox->addItem(player.name.c_str());

        // We make a guess that the person being asked is probably the person whose turn is next but this can be changed
        ui.askedBox->setCurrentText(probableWitness.c_str());
    }

    // Cards stuff
    {
        categoryBoxes = { ui.cat1Box, ui.cat2Box, ui.cat3Box };
        assert(categoryBoxes.size() == NUM_CATEGORIES);

        // Adds the cards in each category to each combo box
        auto& it1 = categoryBoxes.begin();
        for (auto& it2 = pController->cards().begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
        {
            (*it1)->setEnabled(false);  // Set the combo box as disabled until action is chosen

            // Add cards for this category
            for (auto& item : *it2)
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

bool TakeTurn::outcomeChosen()
{
    return ui.outcomeTrue->isChecked() || ui.outcomeFalse->isChecked()
        || (ui.askedButton->isDefault() && (ui.cat1Shown->isChecked() || ui.cat2Shown->isChecked() || ui.cat3Shown->isChecked()));
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
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cat2ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cat3ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::outcomeTrueClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeFalse->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::outcomeFalseClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);

    ui.submitButton->setEnabled(true);
}

void TakeTurn::cancelButtonClicked()
{
    close();
}

void TakeTurn::submitButtonClicked()
{
    const std::vector<Player>& players = pController->players();


    const auto& itDetective = std::find(players.begin(), players.end(), detective);
    if (itDetective == players.end())
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", (str("Failed to find detective player ") + detective).c_str());
        return;
    }


    // Missed a turn
    if (ui.missedButton->isDefault())
    {
        pController->analyseMissed(new Missed(&*itDetective, Action::MISSED));
        return;
    }


    std::vector<Card*> pCards;
    auto& it1 = categoryBoxes.begin();
    for (auto& it2 = pController->cards().begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
    {
        const str card = (*it1)->currentText().toStdString();
        auto& itCard = std::find(it2->begin(), it2->end(), card);
        if (itCard == it2->end())
        {
            // Should never happen
            QMessageBox msgBox;
            msgBox.critical(0, "Error", (str("Failed to find selected card ") + card).c_str());
            return;
        }

        pCards.push_back(&*itCard);
    }


    // Asked a question
    if (ui.askedButton->isDefault())
    {
        const str witness = ui.askedBox->currentText().toStdString();
        const auto& itWitness = std::find(players.begin(), players.end(), witness);
        if (itWitness == players.end())
        {
            // Should never happen
            QMessageBox msgBox;
            msgBox.critical(0, "Error", (str("Failed to find witness player ") + witness).c_str());
            return;
        }


        if (ui.outcomeTrue->isChecked())
            pController->analyseAsked(new Asked(&*itDetective, Action::ASKED, &*itWitness, pCards, true));
        else if (ui.outcomeFalse->isChecked())
            pController->analyseAsked(new Asked(&*itDetective, Action::ASKED, &*itWitness, pCards, false));
        else if (ui.cat1Shown->isChecked())
            pController->analyseAsked(new Asked(&*itDetective, Action::ASKED, &*itWitness, pCards, true, ui.cat1Box->currentText().toStdString()));
        else if (ui.cat2Shown->isChecked())
            pController->analyseAsked(new Asked(&*itDetective, Action::ASKED, &*itWitness, pCards, true, ui.cat2Box->currentText().toStdString()));
        else if (ui.cat3Shown->isChecked())
            pController->analyseAsked(new Asked(&*itDetective, Action::ASKED, &*itWitness, pCards, true, ui.cat3Box->currentText().toStdString()));
        else
        {
            // Should never happen
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "Failed to deduce chosen outcome");
        }

        return;
    }

    // Made a guess
    if (ui.guessedButton->isDefault())
    {
        if (ui.outcomeTrue->isChecked())
            pController->analyseGuessed(new Guessed(&*itDetective, Action::GUESSED, pCards, true));
        else if (ui.outcomeFalse->isChecked())
            pController->analyseGuessed(new Guessed(&*itDetective, Action::GUESSED, pCards, false));

        return;
    }

    // Should never get to here
    QMessageBox msgBox;
    msgBox.critical(0, "Error", "Failed to deduce chosen action");
}
