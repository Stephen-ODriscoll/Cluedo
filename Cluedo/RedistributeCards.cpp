#include "stdafx.h"
#include "RedistributeCards.h"

RedistributeCards::RedistributeCards(Game* pGame, std::shared_ptr<Guessed> pGuessed, std::shared_ptr<const Turn> pOldTurn, QWidget* parent) :
    pGame(pGame),
    pGuessed(pGuessed),
    pOldTurn(pOldTurn),
    QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Redistributed Cards");

    pairs =
    {
        {ui.player1Text, ui.player1CardsBox},
        {ui.player2Text, ui.player2CardsBox},
        {ui.player3Text, ui.player3CardsBox},
        {ui.player4Text, ui.player4CardsBox},
        {ui.player5Text, ui.player5CardsBox}
    };

    size_t i = 0;
    for (Player* pPlayer : g_pPlayersLeft)
    {
        if (pPlayer == pGuessed->pDetective)
            continue;

        pairs[i].first->setText(pPlayer->name.c_str());
        ++i;
    }

    for (; i != pairs.size(); ++i)
    {
        pairs[i].first->hide();
        pairs[i].second->hide();
    }

    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    connect(ui.okayButton, SIGNAL(clicked()), this, SLOT(okayButtonClicked()));
}

void RedistributeCards::cancelButtonClicked()
{
    close();
};

void RedistributeCards::okayButtonClicked()
{
    std::vector<int> redistributedCards;
    for (auto pair : pairs)
        redistributedCards.push_back(pair.second->value());

    pGuessed->redistribedCards = redistributedCards;

    TRY
        pGame->controller.processTurn(pGuessed, pOldTurn);
    CATCH
        
    pGame->refresh();
    close();
};
