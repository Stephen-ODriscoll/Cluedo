#include "stdafx.h"
#include "Home.h"

Home::Home(Controller* pController, QWidget* parent) :
    pController(pController),
    QWidget(parent)
{
    ui.setupUi(this);

    QPixmap pixmap(":/Cluedo/Images/Cluedo.png");
    pixmap = pixmap.scaled(ui.icon->size());
    ui.icon->setPixmap(pixmap);

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
}

void Home::groupModeButtonClicked()
{
    ui.groupModeButton->setDefault(true);
    ui.singleModeButton->setDefault(false);
}

void Home::continueButtonClicked()
{
    Mode mode = Mode::NONE;
    if (ui.singleModeButton->isDefault())
    {
        mode = Mode::SINGLE;
    }
    else if (ui.groupModeButton->isDefault())
    {
        mode = Mode::GROUP;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Mode not selected");
        msgBox.setInformativeText("Please choose either single or group mode");
        msgBox.exec();
        return;
    }

    pController->startGame(mode, ui.numPlayersBox->value());
}
