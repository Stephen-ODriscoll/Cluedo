#include "stdafx.h"
#include "Controller.h"

Controller::Controller(Mode mode, int numPlayers) :
    m_gameOver(false),
    m_mode(mode)
{
    g_numStages = 1;
    g_players.reserve(numPlayers);          // Reallocation breaks pointers
    for (int i = 0; i != numPlayers; ++i)
    {
        g_players.emplace_back();
        g_pPlayersLeft.push_back(&g_players.back());
    }
}

void Controller::initialize(const fs::path& inputFile)
{
    if (!fs::exists(inputFile))
        throw std::invalid_argument(str("Couldn't find file ") + inputFile.string());

    std::ifstream load(inputFile);
    if (!load.is_open())
        throw std::invalid_argument(str("Failed to open file ") + inputFile.string());

    g_categories.clear();
    g_categories.reserve(NUM_CATEGORIES);

    str line;
    std::vector<Card> cards;
    while (std::getline(load, line))
    {
        if (line.empty())
        {
            if (!cards.empty())
            {
                g_categories.emplace_back(cards);
                cards.clear();
            }

            continue;
        }

        std::vector<str> splits = line.split('=');
        cards.emplace_back(splits[0], splits[1], g_categories.size());
    }

    if (!cards.empty())
        g_categories.emplace_back(cards);
    

    if (g_categories.size() != NUM_CATEGORIES)
    {
        if (g_categories.size() < NUM_CATEGORIES)
            throw std::invalid_argument(str("Not enough categories. There should be ") + str(NUM_CATEGORIES));
        
        throw std::invalid_argument(str("Too many categories. There should be ") + str(NUM_CATEGORIES));
    }

    g_progressReport = START_MESSAGE;
}

void Controller::processTurn(std::shared_ptr<const Turn> pNewTurn, std::shared_ptr<const Turn> pOldTurn)
{
    if (pOldTurn)
    {
        auto it = std::find(g_pTurns.begin(), g_pTurns.end(), pOldTurn);
        if (it == g_pTurns.end())
            throw std::exception("Failed to find turn in g_pTurns");

        *it = pNewTurn;
        reAnalyseTurns();
    }
    else
    {
        analyseTurn(pNewTurn);
        g_pTurns.push_back(pNewTurn);
    }
}

void Controller::rename(const Player* pPlayer, const str& newName)
{
    if (pPlayer->name == newName)
        return;

    if (std::find(g_players.begin(), g_players.end(), newName) != g_players.end())
        throw std::exception("Player with that name already exists");

    Player& player = const_cast<Player&>(*pPlayer);
    player.name = newName;
}

void Controller::updatePresets(const Player* pPlayer, std::vector<StagePreset>& newPresets)
{
    Player& player = const_cast<Player&>(*pPlayer);

    for (size_t i = 0; i != newPresets.size(); ++i)
    {
        if (newPresets[i] == player.presets[i])
            continue;

        // If the new number of cards doesn't apply or the new number is less than or equal to the old number
        // and no old cards were removed
        if ((!newPresets[i].isNumCardsKnown() || newPresets[i].numCards <= player.presets[i].numCards) &&
            std::includes(newPresets[i].pCardsOwned.begin(), newPresets[i].pCardsOwned.end(),
            player.presets[i].pCardsOwned.begin(), player.presets[i].pCardsOwned.end()))
        {
            player.presets[i] = newPresets[i];
            for (Card* pCard : player.presets[i].pCardsOwned)
                player.processHas(pCard, g_numStages - 1);
        }
        else
        {
            player.presets[i] = newPresets[i];

            // This info may have been used to make other deductions so we need to start our analysis again
            reAnalyseTurns();
        }
    }
}

void Controller::resetAnalysis()
{
    g_numStages = 1;
    m_gameOver = false;
    g_wrongGuesses.clear();
    g_progressReport = START_MESSAGE;

    // Reset each category
    for (Category& category : g_categories)
        category.reset();

    // Reset each player and add to players left
    g_pPlayersOut.clear();
    g_pPlayersLeft.clear();

    for (Player& player : g_players)
    {
        player.reset();
        g_pPlayersLeft.push_back(&player);
    }
}

void Controller::reAnalyseTurns()
{
    // Start our analysis again
    resetAnalysis();
    for (std::shared_ptr<const Turn> pTurn : g_pTurns)
        analyseTurn(pTurn);
}

void Controller::moveToBack(const Player* pPlayer)
{
    auto it = std::find(g_pPlayersLeft.begin(), g_pPlayersLeft.end(), pPlayer);
    if (it == g_pPlayersLeft.end())
        throw std::exception((str("Failed to find ") + pPlayer->name + str(" in players left")).c_str());

    g_pPlayersLeft.erase(it);
    g_pPlayersLeft.push_back(const_cast<Player*>(pPlayer));
}

void Controller::analyseTurn(std::shared_ptr<const Turn> pTurn)
{
    switch (pTurn->action)
    {
    case Action::MISSED:
        moveToBack(pTurn->pDetective);
        break;

    case Action::ASKED:
        analyseAsked(std::static_pointer_cast<const Asked>(pTurn));
        moveToBack(pTurn->pDetective);
        break;

    case Action::GUESSED:
        analyseGuessed(std::static_pointer_cast<const Guessed>(pTurn));
        break;
    }
}

void Controller::analyseAsked(std::shared_ptr<const Asked> pAsked)
{
    Player& witness = const_cast<Player&>(*pAsked->pWitness);

    if (pAsked->shown)
    {
        if (!pAsked->cardShown.empty())
        {
            auto itCard = std::_Find_pr(pAsked->pCards.begin(), pAsked->pCards.end(), pAsked->cardShown,
                [](const Card* c, const str& s) { return (c->name == s); });
            if (itCard == pAsked->pCards.end())
                throw std::exception("Failed to find card shown");

            witness.processHas(*itCard, g_numStages - 1);
        }
        else
            witness.processHasEither(pAsked->pCards, g_numStages - 1);
    }
    else
        witness.processDoesntHave(pAsked->pCards, g_numStages - 1);
}

void Controller::analyseGuessed(std::shared_ptr<const Guessed> pGuessed)
{
    if (pGuessed->correct)
    {
        m_gameOver = true;
    }
    else
    {
        Player& detective = const_cast<Player&>(*pGuessed->pDetective);

        auto it = std::find(g_pPlayersLeft.begin(), g_pPlayersLeft.end(), &detective);
        if (it == g_pPlayersLeft.end())
            throw std::exception((str("Failed to find ") + detective.name + str(" in players left")).c_str());

        g_pPlayersLeft.erase(it);
        g_pPlayersOut.push_back(&detective);

        // Create new player analyses
        if (pGuessed->redistribedCards.empty())
        {
            for (Player* pPlayerLeft : g_pPlayersLeft)
                pPlayerLeft->processGuessedWrong(&detective);
        }
        else
        {
            for (size_t i = 0; i != g_pPlayersLeft.size(); ++i)
                g_pPlayersLeft[i]->processGuessedWrong(&detective, pGuessed->redistribedCards[i]);
        }

        for (Category& category : g_categories)
            for (Card& card : category.cards)
                card.processGuessedWrong(&detective);

        g_wrongGuesses.push_back(pGuessed->pCards);
        ++g_numStages;
    }
}
