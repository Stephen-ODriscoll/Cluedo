#include "stdafx.h"
#include "Game.h"

Game::Game(Controller* pController, QWidget* parent) :
    pController(pController),
    pPopUp(nullptr),
    QWidget(parent)
{
    ui.setupUi(this);

    QFont font;
    font.setPixelSize(14);
    ui.turnButton->setFont(font);

    ui.upButton->setIcon(QIcon(":/Cluedo/Images/up.png"));
    ui.downButton->setIcon(QIcon(":/Cluedo/Images/down.png"));
    ui.playerInfoButton->setIcon(QIcon(":/Cluedo/Images/rename.png"));

    connect(ui.upButton, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
    connect(ui.downButton, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
    connect(ui.playerInfoButton, SIGNAL(clicked()), this, SLOT(playerInfoButtonClicked()));
    connect(ui.turnButton, SIGNAL(clicked()), this, SLOT(turnButtonClicked()));
    connect(ui.editTurnButton, SIGNAL(clicked()), this, SLOT(editTurnButtonClicked()));

    ui.playersList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.playersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.progressReportText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.progressReportText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui.cardList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.cardList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.playersText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.playersText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.cardInfoList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.cardInfoList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui.turnList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.turnList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

Game::~Game()
{
    delete pPopUp;
}

void Game::startGame()
{
    // Implementation of Bottom-up done using blank boxes (I liked bottom-up better than top-down)
    const std::vector<Player>& players = pController->players();

    ui.playersList->clear();
    for (size_t i = players.size(); i < MAX_PLAYERS; ++i)
        ui.playersList->addItem("");

    for (auto& it = players.rbegin(); it != players.rend(); ++it)
        ui.playersList->addItem(it->name.c_str());

    QFont font;
    font.setPixelSize(22);
    for (int i = 0; i < ui.playersList->count(); ++i)
        ui.playersList->item(i)->setFont(font);

    updateNotes();
}

void Game::updateNotes()
{
    // Notes on cards
    ui.cardList->clear();
    ui.cardInfoList->clear();
    for (std::vector<Card> category : pController->cards())
    {
        for (Card card : category)
        {
            ui.cardList->addItem(card.nickname.c_str());

            str status;
            if (card.ownerKnown())
                ui.cardInfoList->addItem(card.pOwner->name.c_str());
            else
                ui.cardInfoList->addItem(convictionStrings.find(card.conviction)->second.c_str());
        }

        ui.cardList->addItem("");
        ui.cardInfoList->addItem("");
    }

    ui.cardList->takeItem(ui.cardList->count() - 1);
    ui.cardInfoList->takeItem(ui.cardInfoList->count() - 1);

    // Notes on players
    str status;
    for (const Analysis& analysis : pController->analyses())
        status += analysis.to_str(pController->numStages());

    ui.playersText->setPlainText(status.c_str());

    // Notes on turns
    ui.turnList->clear();
    for (std::shared_ptr<const Turn> turn : pController->turns())
        ui.turnList->addItem(QString(turn->to_str().c_str()));

    bool empty = !ui.turnList->count();
    ui.editTurnButton->setEnabled(!empty);
    if (empty)
        ui.turnList->addItem(QString("No turns to show yet"));

    // Font stuff
    QFont font;
    font.setPixelSize(14);
    for (int i = 0; i < ui.cardList->count(); ++i)
        ui.cardList->item(i)->setFont(font);

    for (int i = 0; i < ui.cardInfoList->count(); ++i)
        ui.cardInfoList->item(i)->setFont(font);

    font.setPixelSize(12);
    ui.playersText->setFont(font);
}

void Game::moveToBack(const str& playerName)
{
    int index = findItem(playerName);
    if (index == -1)
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to find player with that name");
        return;
    }

    ui.playersList->insertItem(MAX_PLAYERS - pController->playersLeft(), ui.playersList->takeItem(index));
}

void Game::removePlayer(const str& playerName)
{
    int index = findItem(playerName);
    if (index == -1)
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to find player with that name");
        return;
    }

    ui.playersList->takeItem(index);
    ui.playersList->insertItem(0, "");
    
    QFont font;
    font.setPixelSize(22);
    ui.playersList->item(0)->setFont(font);
}

void Game::editName(const str& oldName, const str& newName)
{
    int index = findItem(oldName);
    if (index == -1)
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to find player with that name");
        return;
    }

    ui.playersList->item(index)->setText(newName.c_str());
    updateNotes();
}

int Game::findItem(const str& playerName)
{
    // Search backwards as we're more likely to be dealing with the bottom items
    int i = ui.playersList->count() - 1;
    for (; i != MAX_PLAYERS - pController->playersLeft(); --i)
        if (ui.playersList->item(i)->text() == playerName.c_str())
            return i;

    return -1;
}

void Game::upButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    if (row <= int(MAX_PLAYERS - pController->playersLeft()))
        return;

    ui.playersList->insertItem(row, ui.playersList->takeItem(row - 1));
}

void Game::downButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    if (row < int(MAX_PLAYERS - pController->playersLeft()) || row == MAX_PLAYERS - 1)
        return;

    ui.playersList->insertItem(row, ui.playersList->takeItem(row + 1));
}

void Game::playerInfoButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    if (row < int(MAX_PLAYERS - pController->playersLeft()))
        return;

    const std::vector<Player>& players = pController->players();
    auto it = std::find(players.begin(), players.end(), ui.playersList->item(row)->text().toStdString());
    if (it == players.end())
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", "Failed to find the chosen player");
        return;
    }

    delete pPopUp;

    pPopUp = new PlayerInfo(pController, this, &*it);
    pPopUp->show();
}

void Game::turnButtonClicked()
{
    delete pPopUp;

    pPopUp = new TakeTurn(pController,
        ui.playersList->item(MAX_PLAYERS - 1)->text().toStdString(),
        ui.playersList->item(MAX_PLAYERS - 2)->text().toStdString());
    pPopUp->show();
}

void Game::editTurnButtonClicked()
{
    int row = ui.turnList->currentIndex().row();
    if (row == -1)
        return;

    const std::vector<std::shared_ptr<const Turn>>& turns = pController->turns();

    auto it = std::lower_bound(turns.begin(), turns.end(), row + 1,
        [](std::shared_ptr<const Turn> item, int target) -> bool { return item->id < target; });
    if (it == turns.end())
    {
        // Should never happen
        QMessageBox msgBox;
        msgBox.critical(0, "Error", (str("Failed to find turn with id ") + str(row + 1)).c_str());
        return;
    }
    
    delete pPopUp;
    pPopUp = new TakeTurn(pController, *it);
    pPopUp->show();
}
