#include "stdafx.h"
#include "Controller.h"

Controller::Controller(Game* pGame, Mode mode, int numPlayers) :
    m_gameOver(false),
    m_mode(mode),
    m_pGame(pGame)
{
    g_numStages = 1;
    for (int i = 0; i != numPlayers; ++i)
    {
        g_players.emplace_back();
        g_pPlayersLeft.insert(&g_players.back());
    }

    // Keep trying until we successfully load the file
    for (fs::path inputFile = "Cluedo.txt"; true;)
    {
        try
        {
            if (!fs::exists(inputFile))
                throw std::invalid_argument(str("Couldn't find file ") + inputFile.string());

            std::ifstream load(inputFile);
            if (!load.is_open())
                throw std::invalid_argument(str("Failed to open file ") + inputFile.string());

            str line;
            g_cards.emplace_back();
            for (uint32_t category = 0U; std::getline(load, line);)
            {
                if (NUM_CATEGORIES <= category)
                    throw std::invalid_argument(str("Too many categories. There should be ") + str(NUM_CATEGORIES));

                if (line.empty())
                {
                    if (!g_cards[category].empty())
                    {
                        g_cards.emplace_back();
                        ++category;
                    }

                    continue;
                }

                std::vector<str> splits = line.split('=');
                g_cards[category].push_back(Card(splits[0], splits[1], category));
            }

            if (g_cards.size() != NUM_CATEGORIES)
                throw std::invalid_argument(str("Not enough categories. There should be ") + str(NUM_CATEGORIES));

            break;          // This is how we complete the constructor
        }
        catch (const std::invalid_argument& ex)
        {
            g_cards.clear();

            inputFile = fs::path(m_pGame->openCluedoTextFile(ex.what()));
            if (inputFile.empty())
                exit(EXIT_SUCCESS);     // User clicked cancel
        }
    }
}

void Controller::processNewTurn(std::shared_ptr<const Turn> pTurn)
{
    g_pTurns.push_back(pTurn);

    try
    {
        analyseTurn(pTurn);
    }
    catch (const contradiction& ex) { m_pGame->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGame->critical("Exception Occured", ex.what()); }

    m_pGame->updateNotes();
}

bool Controller::rename(const Player* pPlayer, const str& newName)
{
    try
    {
        if (pPlayer->name == newName)
            return false;

        if (std::find(g_players.begin(), g_players.end(), newName) != g_players.end())
            throw std::exception("Player with that name already exists");

        m_pGame->editName(pPlayer->name, newName);

        Player& player = const_cast<Player&>(*pPlayer);
        player.name = newName;
        return true;
    }
    catch (const std::exception& ex) { m_pGame->critical("Exception Occured", ex.what()); }

    return false;
}

void Controller::updateHasCards(const Player* pPlayer, const std::vector<str>& cardNames, const size_t stageIndex)
{
    try
    {
        Player& player = const_cast<Player&>(*pPlayer);

        std::set<Card*> newCardsOwned;
        for (const str& cardName : cardNames)
        {
            bool found = false;
            for (std::vector<Card>& category : g_cards)
            {
                auto it = std::find(category.begin(), category.end(), cardName);
                if (it == category.end())
                    continue;

                found = true;
                newCardsOwned.insert(&*it);
                break;
            }

            if (!found)
                throw std::exception((str("Failed to find card ") + cardName).c_str());
        }

        std::set<Card*>& pCardsOwned = player.stagePresets[stageIndex].pCardsOwned;
        if (std::includes(pCardsOwned.begin(), pCardsOwned.end(),
            newCardsOwned.begin(), newCardsOwned.end()))
        {
            pCardsOwned = newCardsOwned;
            for (Card* pCard : pCardsOwned)
            {
                if (player.processHas(pCard, g_numStages - 1))
                    continueDeducing();
            }
        }
        else
        {
            pCardsOwned = newCardsOwned;

            // This info may have been used to make other deductions so we need to start our analysis again
            reAnalyseTurns();
        }
    }
    catch (const contradiction& ex) { m_pGame->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGame->critical("Exception Occured", ex.what()); }

    m_pGame->updateNotes();
}

void Controller::replaceTurn(std::shared_ptr<const Turn> oldTurn, std::shared_ptr<const Turn> newTurn)
{
    try
    {
        auto it = std::find(g_pTurns.begin(), g_pTurns.end(), oldTurn);
        if (it == g_pTurns.end())
            throw std::exception("Failed to find old turn in turns vector");

        *it = newTurn;      // Replace this turn with the updated one

        // Our analysis is may now be wrong so we need to start new
        reAnalyseTurns();
    }
    catch (const contradiction& ex) { m_pGame->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGame->critical("Exception Occured", ex.what()); }

    m_pGame->updateNotes();
}

size_t Controller::numStages()
{
    return g_numStages;
}

void Controller::analysesSetup()
{
    g_numStages = 1;
    m_gameOver = false;

    g_pPlayersOut.clear();
    for (Player& player : g_players)
    {
        player.reset();
        g_pPlayersLeft.insert(&player);
    }

    // reset each cards owner and conviction
    for (std::vector<Card>& category : g_cards)
        for (Card& card : category)
            card.reset();
}

void Controller::reAnalyseTurns()
{
    // Start our analysis again
    analysesSetup();
    for (std::shared_ptr<const Turn> pTurn : g_pTurns)
        analyseTurn(pTurn);
}

void Controller::analyseTurn(std::shared_ptr<const Turn> pTurn)
{
    switch (pTurn->action)
    {
    case Action::MISSED:
        m_pGame->moveToBack(pTurn->pDetective->name);
        break;

    case Action::ASKED:
        analyseAsked(std::static_pointer_cast<const Asked>(pTurn));
        m_pGame->moveToBack(pTurn->pDetective->name);
        break;

    case Action::GUESSED:
        analyseGuessed(std::static_pointer_cast<const Guessed>(pTurn));
        if (!m_gameOver)
            m_pGame->removePlayerAndAddStage(pTurn->pDetective->name);
        break;
    }
}

void Controller::analyseAsked(std::shared_ptr<const Asked> pAsked)
{
    Player& witness = const_cast<Player&>(*pAsked->pWitness);

    bool cardDeduced;
    if (pAsked->shown)
    {
        if (!pAsked->cardShown.empty())
        {
            auto itCard = std::_Find_pr(pAsked->pCards.begin(), pAsked->pCards.end(), pAsked->cardShown,
                [](const Card* c, const str& s) { return (c->name == s); });
            if (itCard == pAsked->pCards.end())
                throw std::exception("Card shown not found amongst cards");

            cardDeduced = witness.processHas(*itCard, g_numStages - 1);
        }
        else
            cardDeduced = witness.processHasEither(pAsked->pCards, g_numStages - 1);
    }
    else
        cardDeduced = witness.processDoesntHave(pAsked->pCards, g_numStages - 1) || exteriorChecks();

    if (cardDeduced)
        continueDeducing();
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

        g_pPlayersLeft.erase(&detective);
        g_pPlayersOut.push_back(&detective);

        // Create new analysis for every player left
        for (Player* player : g_pPlayersLeft)
            player->processGuessedWrong(&detective);

        for (std::vector<Card>& category : g_cards)
            for (Card& card : category)
                card.processGuessedWrong(&detective);

        ++g_numStages;
        g_wrongGuesses.push_back(pGuessed->pCards);
    }
}

void Controller::continueDeducing()
{
    // This could loop a few times. One deduction could lead to another and so on
    bool cardDeduced = true;
    while (cardDeduced)
    {
        cardDeduced = false;
        for (auto it = g_players.begin(); it != g_players.end(); ++it)
            cardDeduced |= it->recheck();

        cardDeduced |= exteriorChecks();
    }
}

bool Controller::exteriorChecks()
{
    bool cardDeduced = false;
    for (std::vector<Card>& category : g_cards)
    {
        bool guiltyKnown = false;
        std::vector<Card*> unknownCards;
        for (Card& card : category)
        {
            if (card.isUnknown())
                unknownCards.push_back(&card);
            else if (card.isGuilty())
                guiltyKnown = true;
        }

        if (!guiltyKnown)
        {
            switch (unknownCards.size())
            {
            case 0:
                throw contradiction((str("Ruled out all cards in category starting with ") + category.front().name).c_str());

            case 1:
                // All other cards have been ruled out so this card must be the murder card
                cardDeduced = true;
                unknownCards.front()->conviction = Conviction::GUILTY;
                continue;
            }
        }

        for (size_t i = 0; i != g_numStages; ++i)
        {
            for (Card* pUnknownCard : unknownCards)
            {
                switch (pUnknownCard->stages[i].pPossibleOwners.size())
                {
                case 0:
                    if (guiltyKnown)
                        throw contradiction((pUnknownCard->name + str(" has been convicted but another card in the same category was already covicted")).c_str());

                    // Nobody can have this card so it must be guilty
                    cardDeduced = true;
                    guiltyKnown = true;
                    pUnknownCard->conviction = Conviction::GUILTY;
                    break;
                case 1:
                    // We know which card is guilty and nobody else can have this card
                    if (guiltyKnown)
                        cardDeduced = (*pUnknownCard->stages[i].pPossibleOwners.begin())->processHas(pUnknownCard, i);
                }
            }
        }
    }

    return cardDeduced;
}
