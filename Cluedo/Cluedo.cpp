#include "stdafx.h"
#include "Cluedo.h"

Cluedo::Cluedo(QWidget *parent) :
    QMainWindow(parent),
    pHome(new Home(this)),
    pGame(nullptr)

{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    
    ui.stackedWidget->addWidget(pHome);
}

Cluedo::~Cluedo()
{
    delete pHome;
    delete pGame;
}

void Cluedo::startGame(Mode mode, int numPlayers)
{
    pGame = new Game(mode, numPlayers);

    ui.stackedWidget->addWidget(pGame);
    ui.stackedWidget->setCurrentWidget(pGame);
}
