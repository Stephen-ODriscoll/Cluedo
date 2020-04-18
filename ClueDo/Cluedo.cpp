#include "stdafx.h"
#include "Cluedo.h"
#include "Global.h"

Cluedo::Cluedo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QPixmap pixmap(":/Cluedo/Images/Cluedo.png");
    pixmap = pixmap.scaled(ui.icon->size());
    ui.icon->setPixmap(pixmap);

    connect(ui.singleModeButton, SIGNAL(clicked()), this, SLOT(singleModeButtonClicked()));
    connect(ui.groupModeButton, SIGNAL(clicked()), this, SLOT(groupModeButtonClicked()));

    ui.numPlayersBox->setRange(MIN_PLAYERS, MAX_PLAYERS);
    ui.numPlayersBox->setValue(MIN_PLAYERS);
}

void Cluedo::singleModeButtonClicked()
{
    ui.singleModeButton->setDefault(true);
    ui.groupModeButton->setDefault(false);
}

void Cluedo::groupModeButtonClicked()
{
    ui.groupModeButton->setDefault(true);
    ui.singleModeButton->setDefault(false);
}
