#include "stdafx.h"
#include "Cluedo.h"

Cluedo::Cluedo(QWidget *parent) :
    QMainWindow(parent),
    pController(new Controller(this)),
    pHome(new Home(pController)),
    pGame(new Game(pController))

{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    
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

void Cluedo::critical(const str& title, const str& desc)
{
    QMessageBox msgBox;
    msgBox.critical(0, title.c_str(), desc.c_str());
}

Game* Cluedo::game()
{
    return pGame;
}
