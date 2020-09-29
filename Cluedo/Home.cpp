#include "stdafx.h"
#include "Home.h"

Home::Home(Cluedo* pCluedo, QWidget* parent) :
    pCluedo(pCluedo),
    QWidget(parent)
{
    ui.setupUi(this);

    QPixmap pixmap(":/Cluedo/Images/Cluedo.png");
    pixmap = pixmap.scaled(ui.icon->size());
    ui.icon->setPixmap(pixmap);

    ui.continueButton->setEnabled(false);

    connect(ui.singleModeButton, SIGNAL(clicked()), this, SLOT(singleModeButtonClicked()));
    connect(ui.groupModeButton, SIGNAL(clicked()), this, SLOT(groupModeButtonClicked()));

    ui.numPlayersBox->setRange(MIN_PLAYERS, MAX_PLAYERS);
    ui.numPlayersBox->setValue(MIN_PLAYERS);

    connect(ui.continueButton, SIGNAL(clicked()), this, SLOT(continueButtonClicked()));
}

void Home::singleModeButtonClicked()
{
    ui.singleModeButton->setDefault(true);
    ui.groupModeButton->setDefault(false);

    ui.continueButton->setEnabled(true);
}

void Home::groupModeButtonClicked()
{
    ui.groupModeButton->setDefault(true);
    ui.singleModeButton->setDefault(false);

    ui.continueButton->setEnabled(true);
}

void Home::continueButtonClicked()
{
    Mode mode = Mode::NONE;
    if (ui.singleModeButton->isDefault())
        mode = Mode::SINGLE;
    else if (ui.groupModeButton->isDefault())
        mode = Mode::GROUP;
    else
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to deduce chosen mode");
        return;
    }

    pCluedo->startGame(mode, ui.numPlayersBox->value());
}
