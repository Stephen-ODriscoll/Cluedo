#include "stdafx.h"
#include "Cluedo.h"

Cluedo::Cluedo(QWidget *parent) :
    QMainWindow(parent),
    pController(new Controller(this)),
    pHome(new Home(pController)),
    pGame(new Game(pController))

{
    ui.setupUi(this);
    
    ui.stackedWidget->addWidget(pHome);
    ui.stackedWidget->addWidget(pGame);
}

Cluedo::~Cluedo()
{
    delete pController;
    delete pHome;
    delete pGame;
}

str Cluedo::openCluedoTextFile(const str& issue)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Woops...");
    msgBox.setText((str("While trying to load Cluedo.txt - ") + issue).c_str());
    msgBox.exec();

    return QFileDialog::getOpenFileName(this, tr("Open Cluedo.txt"), QDir::currentPath(), tr("Text files (*.txt)")).toStdString();
}

void Cluedo::startGame()
{
    ui.stackedWidget->setCurrentWidget(pGame);
    pGame->startGame();
}

Game* Cluedo::game()
{
    return pGame;
}
