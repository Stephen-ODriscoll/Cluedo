#include "stdafx.h"
#include "Game.h"

Game::Game(Controller* pController, QWidget* parent) :
    pController(pController),
    pTakeTurn(nullptr),
    QWidget(parent)
{
    ui.setupUi(this);

    ui.upButton->setIcon(QIcon(":/Cluedo/Images/up.png"));
    ui.downButton->setIcon(QIcon(":/Cluedo/Images/down.png"));
    ui.renameButton->setIcon(QIcon(":/Cluedo/Images/rename.png"));

    connect(ui.upButton, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
    connect(ui.downButton, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
    connect(ui.renameButton, SIGNAL(clicked()), this, SLOT(renameButtonClicked()));
    connect(ui.turnButton, SIGNAL(clicked()), this, SLOT(turnButtonClicked()));
}

Game::~Game()
{
    delete pTakeTurn;
}

void Game::startGame()
{
    const std::vector<Player>& players = pController->players();

    // Implementation of Bottom-up done using blank boxes (I liked bottom-up better than top-down)
    for (int i = players.size(); i < MAX_PLAYERS; ++i)
        ui.playersList->addItem("");

    for (auto& it = players.rbegin(); it != players.rend(); ++it)
        ui.playersList->addItem(it->name.c_str());

    QFont font;
    font.setPointSize(14);
    for (int i = 0; i < ui.playersList->count(); ++i)
        ui.playersList->item(i)->setFont(font);
}

void Game::upButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    
    if (row <= MAX_PLAYERS - pController->players().size())
        return;

    ui.playersList->insertItem(row, ui.playersList->takeItem(row - 1));
}

void Game::downButtonClicked()
{
    int row = ui.playersList->currentIndex().row();

    if (row < MAX_PLAYERS - pController->players().size() || row == MAX_PLAYERS - 1)
        return;

    ui.playersList->insertItem(row, ui.playersList->takeItem(row + 1));
}

void Game::renameButtonClicked()
{
    int row = ui.playersList->currentIndex().row();

    if (row < MAX_PLAYERS - pController->players().size())
        return;

    bool dialogResult;
    QInputDialog inDialog;
    QString result = inDialog.getText(0, "", "New name:", QLineEdit::Normal,
        ui.playersList->item(row)->text(), &dialogResult);

    if (!dialogResult)
        return;

    if (!pController->rename(ui.playersList->item(row)->text().toStdString(), result.toStdString()))
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to find corresponding player");
        return;
    }

    ui.playersList->takeItem(row);
    ui.playersList->insertItem(row, result);
    
    QFont font;
    font.setPointSize(14);
    ui.playersList->item(row)->setFont(font);
}

void Game::turnButtonClicked()
{
    delete pTakeTurn;

    pTakeTurn = new TakeTurn(pController,
        ui.playersList->item(MAX_PLAYERS - 1)->text().toStdString(),
        ui.playersList->item(MAX_PLAYERS - 2)->text().toStdString());
    pTakeTurn->show();
}
