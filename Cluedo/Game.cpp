#include "stdafx.h"
#include "Game.h"

Game::Game(Mode mode, int numPlayers, QWidget* parent) :
    controller(mode, numPlayers),
    pPopUp(nullptr),
    stageDisplayed(1),
    QWidget(parent)
{
    ui.setupUi(this);

    for (fs::path inputFile = "Cluedo.txt"; true; )
    {
        try
        {
            controller.initialize(inputFile);
            break;
        }
        catch (const std::invalid_argument& ex)
        {
            inputFile = openCluedoTextFile(ex.what());

            if (inputFile.empty())
                exit(EXIT_SUCCESS);     // User clicked cancel
        }
    }

    // Font stuff
    QFont font;
    font.setPixelSize(22);
    ui.playersList->setFont(font);
    ui.statusLabel->setFont(font);
    ui.stageLabel->setFont(font);
    
    font.setPixelSize(14);
    ui.stageBox->setFont(font);
    ui.turnButton->setFont(font);
    ui.cardList->setFont(font);
    ui.cardInfoList->setFont(font);

    font.setPixelSize(12);
    ui.playersText->setFont(font);

    // Setting icons
    ui.upButton->setIcon(QIcon(":/Cluedo/Images/up.png"));
    ui.downButton->setIcon(QIcon(":/Cluedo/Images/down.png"));
    ui.playerInfoButton->setIcon(QIcon(":/Cluedo/Images/rename.png"));

    connect(ui.upButton, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
    connect(ui.downButton, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
    connect(ui.playerInfoButton, SIGNAL(clicked()), this, SLOT(playerInfoButtonClicked()));
    connect(ui.turnButton, SIGNAL(clicked()), this, SLOT(turnButtonClicked()));
    connect(ui.editTurnButton, SIGNAL(clicked()), this, SLOT(editTurnButtonClicked()));
    connect(ui.playersList, SIGNAL(currentRowChanged(int)), this, SLOT(playersListRowChanged(int)));
    connect(ui.stageBox, SIGNAL(currentTextChanged(QString)), this, SLOT(stageBoxChanged(QString)));
    connect(ui.hideBox, SIGNAL(stateChanged(int)), this, SLOT(hideBoxStageChanged(int)));

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

    // Constant card names
    for (const Category& category : g_categories)
    {
        for (const Card& card : category.cards)
            ui.cardList->addItem(card.nickname.c_str());

        ui.cardList->addItem("");
    }

    ui.cardList->takeItem(ui.cardList->count() - 1);

    ui.stageBox->addItem("1");
    ui.statusLabel->setText(statusStrings.find(Status::NEEDS_CARD_INFO)->second.c_str());

    refresh();
}

Game::~Game()
{
    delete pPopUp;
}

void Game::refresh()
{
    // Prevent multiple refreshing (Changing the stage index will trigger a second refresh)
    if (isRefreshing)
        return;

    struct Refresher
    {
        bool& b;

        Refresher(bool& b) : b(b) { b = true; }
        ~Refresher() { b = false; }
    };

    Refresher refresher(isRefreshing);
    {
        // Overview on players (Bottom-up done using blank boxes)
        ui.playersList->clear();
        for (size_t i = g_pPlayersLeft.size(); i < MAX_PLAYERS; ++i)
            ui.playersList->addItem("");

        for (auto& it = g_pPlayersLeft.rbegin(); it != g_pPlayersLeft.rend(); ++it)
            ui.playersList->addItem((*it)->name.c_str());

        // Overview on stages
        if (ui.stageBox->count() != g_numStages)
        {
            while (g_numStages < ui.stageBox->count())
                ui.stageBox->removeItem(ui.stageBox->count() - 1);

            while (ui.stageBox->count() < g_numStages)
                ui.stageBox->addItem(str(ui.stageBox->count() + 1).c_str());

            ui.stageBox->setCurrentIndex(ui.stageBox->count() - 1);
        }

        // Turns
        ui.turnList->clear();
        for (size_t i = 0; i != g_pTurns.size(); ++i)
            ui.turnList->addItem((str(i + 1) + str(".) ") + g_pTurns[i]->to_str()).c_str());

        bool empty = !ui.turnList->count();
        ui.editTurnButton->setEnabled(!empty);
        if (empty)
            ui.turnList->addItem("No turns to show yet");

        // Hide the actual information if user selects it
        if (hide)
        {
            ui.progressReportText->clear();
            ui.cardInfoList->clear();
            ui.playersText->clear();
            return;
        }

        // Progress report
        ui.progressReportText->setText(g_progressReport.c_str());

        // Notes on cards
        ui.cardInfoList->clear();
        for (const Category& category : g_categories)
        {
            for (const Card& card : category.cards)
            {
                if (card.ownerKnown(stageDisplayed - 1))
                    ui.cardInfoList->addItem(card.stages[stageDisplayed - 1].pOwner->name.c_str());
                else
                    ui.cardInfoList->addItem(convictionStrings.find(card.conviction)->second.c_str());
            }

            ui.cardInfoList->addItem("");
        }

        ui.cardInfoList->takeItem(ui.cardInfoList->count() - 1);

        // Notes on players
        ui.playersText->clear();
        for (const Player& player : g_players)
            ui.playersText->appendPlainText(player.to_str(stageDisplayed - 1).c_str());
    }
}

const fs::path Game::openCluedoTextFile(const str& issue)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Woops...");
    msgBox.setText((str("While trying to load Cluedo.txt - ") + issue).c_str());
    msgBox.exec();

    return QFileDialog::getOpenFileName(this, tr("Open Cluedo.txt"), QDir::currentPath(), tr("Text files (*.txt)")).toStdWString();
}

void Game::upButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    ui.playersList->insertItem(row, ui.playersList->takeItem(row - 1));

    playersListRowChanged(ui.playersList->currentIndex().row());
}

void Game::downButtonClicked()
{
    int row = ui.playersList->currentIndex().row();
    ui.playersList->insertItem(row, ui.playersList->takeItem(row + 1));

    playersListRowChanged(ui.playersList->currentIndex().row());
}

void Game::playerInfoButtonClicked()
{
    int row = ui.playersList->currentIndex().row();

    auto it = std::find(g_players.begin(), g_players.end(), ui.playersList->item(row)->text().toStdString());
    if (it == g_players.end())
    {
        // Should never happen
        CRITICAL("Error", "Failed to find the chosen player")
        return;
    }

    delete pPopUp;

    pPopUp = new PlayerInfo(this, &*it, stageDisplayed);
    pPopUp->show();
}

void Game::turnButtonClicked()
{
    delete pPopUp;

    pPopUp = new TakeTurn(this,
        ui.playersList->item(MAX_PLAYERS - 1)->text().toStdString(),
        ui.playersList->item(MAX_PLAYERS - 2)->text().toStdString());
    pPopUp->show();
}

void Game::editTurnButtonClicked()
{
    int row = ui.turnList->currentIndex().row();
    if (row == -1)
        return;
    
    delete pPopUp;

    pPopUp = new TakeTurn(this, g_pTurns[row]);
    pPopUp->show();
}

void Game::playersListRowChanged(int row)
{
    if (row == -1)
    {
        ui.upButton->setEnabled(false);
        ui.downButton->setEnabled(false);
        ui.playerInfoButton->setEnabled(false);
    }
    else
    {
        ui.upButton->setEnabled(int(MAX_PLAYERS - g_pPlayersLeft.size()) < row);
        ui.downButton->setEnabled(int(MAX_PLAYERS - g_pPlayersLeft.size()) <= row && row != MAX_PLAYERS - 1);
        ui.playerInfoButton->setEnabled(int(MAX_PLAYERS - g_pPlayersLeft.size()) <= row);
    }
}

void Game::stageBoxChanged(const QString& text)
{
    stageDisplayed = str(text.toStdString()).toull();
    refresh();
}

void Game::hideBoxStageChanged(int state)
{
    hide = ui.hideBox->isChecked();
    refresh();
}
