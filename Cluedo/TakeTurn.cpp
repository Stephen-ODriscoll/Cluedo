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
}

void TakeTurn::cat1ShownClicked()
{
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);
}

void TakeTurn::cat2ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);
}

void TakeTurn::cat3ShownClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
    ui.outcomeFalse->setChecked(false);
}

void TakeTurn::outcomeTrueClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeFalse->setChecked(false);
}

void TakeTurn::outcomeFalseClicked()
{
    ui.cat1Shown->setChecked(false);
    ui.cat2Shown->setChecked(false);
    ui.cat3Shown->setChecked(false);
    ui.outcomeTrue->setChecked(false);
}

void TakeTurn::cancelButtonClicked()
{
    close();
}

void TakeTurn::submitButtonClicked()
{
    Action action;
    if (ui.missedButton->isDefault())
        action = Action::MISSED;
    else if (ui.askedButton->isDefault())
        action = Action::ASKED;
    else if (ui.guessedButton->isDefault())
        action = Action::GUESSED;
    else
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Action not selected");
        msgBox.setInformativeText("Please choose either missed a turn, asked or guessed");
        msgBox.exec();
        return;
    }

    //pController->analyseTurn(Turn());
}
