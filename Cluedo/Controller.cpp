#include "stdafx.h"
#include "Controller.h"

Controller::Controller(Cluedo* pGUI, fs::path inputFile) :
    m_mode(Mode::NONE),
    m_gameOver(false),
    m_numStages(1),
    m_pGUI(pGUI)
{
    // Keep trying until we successfully load the file
    while (true)
    {
        try
        {
            if (!fs::exists(inputFile))
                throw std::invalid_argument(str("Couldn't find file ") + inputFile.string());

            std::ifstream load(inputFile);
            if (!load.is_open())
                throw std::invalid_argument(str("Failed to open file ") + inputFile.string());

            str line;
            m_cards.push_back(std::vector<Card>());
            for (uint32_t category = 0U; std::getline(load, line);)
            {
                if (NUM_CATEGORIES <= category)
                    throw std::invalid_argument(str("Too many categories. There should be ") + str(NUM_CATEGORIES));

                if (line.empty())
                {
                    if (!m_cards[category].empty())
                    {
                        m_cards.push_back(std::vector<Card>());
                        ++category;
                    }

                    continue;
                }

                // ++m_numCards;
                std::vector<str> splits = line.split('=');
                m_cards[category].push_back(Card(splits[0], splits[1]));
            }

            if (m_cards.size() != NUM_CATEGORIES)
                throw std::invalid_argument(str("Not enough categories. There should be ") + str(NUM_CATEGORIES));

            break;          // This is how we complete the constructor
        }
        catch (const std::invalid_argument& ex)
        {
            m_cards.clear();

            inputFile = fs::path(m_pGUI->openCluedoTextFile(ex.what()));
            if (inputFile.empty())
                exit(EXIT_SUCCESS);     // User clicked cancel
        }
    }
}

void Controller::startGame(Mode mode, int numPlayers)
{
    m_mode = mode;

    for (int i = 0; i < numPlayers; ++i)
        m_players.push_back(Player());

    analysesSetup();
    m_pGUI->showGame();
}

void Controller::processNewTurn(std::shared_ptr<const Turn> pTurn)
{
    m_pTurns.push_back(pTurn);

    try
    {
        analyseTurn(pTurn);
    }
    catch (const contradiction& ex) { m_pGUI->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    m_pGUI->game()->updateNotes();
}

bool Controller::rename(const Player* pPlayer, const str& newName)
{
    try
    {
        if (pPlayer->name == newName)
            return false;

        if (std::find(m_players.begin(), m_players.end(), newName) != m_players.end())
            throw std::exception("Player with that name already exists");

        Player& player = const_cast<Player&>(*pPlayer);
        player.updateName(newName);
        m_pGUI->game()->editName(pPlayer->name, newName);
        return true;
    }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    return false;
}

void Controller::hasCard(const Player* pPlayer, const str& cardName)
{
    try
    {
        Player& player = const_cast<Player&>(*pPlayer);

        Card* pCard = nullptr;
        for (std::vector<Card>& category : m_cards)
        {
            auto it = std::find(category.begin(), category.end(), cardName);
            if (it == category.end())
                continue;

            pCard = &*it;
            player.pCardsOwned.insert(pCard);
            break;
        }

        if (!pCard)
            throw std::exception((str("Failed to find card ") + cardName).c_str());

        auto it = std::find(m_analyses.begin(), m_analyses.end(), &player);
        if (it == m_analyses.end())
            throw std::exception((str("Failed to find analysis for player ") + player.name).c_str());

        // If this is new information see if we can make anymore deductions
        if (it->processHas(pCard))
            continueDeducing();
    }
    catch (const contradiction& ex) { m_pGUI->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    m_pGUI->game()->updateNotes();
}

void Controller::removeHasCard(const Player* pPlayer, const str& cardName)
{
    try
    {
        Player& player = const_cast<Player&>(*pPlayer);

        bool found = false;
        for (std::vector<Card>& category : m_cards)
        {
            auto it = std::find(category.begin(), category.end(), cardName);
            if (it == category.end())
                continue;

            found = true;
            player.pCardsOwned.erase(&*it);
            break;
        }

        if (!found)
            throw std::exception((str("Failed to find card ") + cardName).c_str());

        // This info may have been used to make other deductions so we need to start our analysis again
        reAnalyseTurns();
    }
    catch (const contradiction& ex) { m_pGUI->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    m_pGUI->game()->updateNotes();
}

void Controller::replaceTurn(std::shared_ptr<const Turn> oldTurn, std::shared_ptr<const Turn> newTurn)
{
    try
    {
        auto it = std::find(m_pTurns.begin(), m_pTurns.end(), oldTurn);
        if (it == m_pTurns.end())
            throw std::exception("Failed to find old turn in turns vector");

        *it = newTurn;      // Replace this turn with the updated one

        // Our analysis is may now be wrong so we need to start new
        reAnalyseTurns();
    }
    catch (const contradiction& ex) { m_pGUI->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    m_pGUI->game()->updateNotes();
}

size_t Controller::numStages()
{
    return m_numStages;
}

size_t Controller::playersLeft()
{
    return m_players.size() - m_numStages + 1;
}

std::vector<std::vector<Card>>& Controller::cards()
{
    return m_cards;
}

const std::vector<Analysis>& Controller::analyses()
{
    return m_analyses;
}

const std::vector<Player>& Controller::players()
{
    return m_players;
}

const std::vector<std::shared_ptr<const Turn>>& Controller::turns()
{
    return m_pTurns;
}

void Controller::analysesSetup()
{
    m_numStages = 1;
    m_gameOver = false;

    // reset each cards owner and conviction
    for (std::vector<Card>& category : m_cards)
        for (Card& card : category)
            card.reset();

    m_analyses.clear();
    for (Player& player : m_players)
    {
        m_analyses.emplace_back(&player);

        for (Card* pCard : player.pCardsOwned)
            m_analyses.back().processHas(pCard);
    }

    m_pGUI->game()->startGame();
}

void Controller::reAnalyseTurns()
{
    // Start our analysis again
    analysesSetup();
    for (std::shared_ptr<const Turn> pTurn : m_pTurns)
        analyseTurn(pTurn);
}

void Controller::analyseTurn(std::shared_ptr<const Turn> pTurn)
{
    switch (pTurn->action)
    {
    case Action::MISSED:
        m_pGUI->game()->moveToBack(pTurn->pDetective->name);
        break;

    case Action::ASKED:
        analyseAsked(std::static_pointer_cast<const Asked>(pTurn));
        m_pGUI->game()->moveToBack(pTurn->pDetective->name);
        break;

    case Action::GUESSED:
        analyseGuessed(std::static_pointer_cast<const Guessed>(pTurn));
        if (!m_gameOver)
            m_pGUI->game()->removePlayer(pTurn->pDetective->name);
        break;
    }
}

void Controller::analyseAsked(std::shared_ptr<const Asked> pAsked)
{
    auto it = std::find(m_analyses.begin(), m_analyses.end(), pAsked->pWitness);
    if (it == m_analyses.end())
        throw std::exception((str("Failed to find analysis for player ") + pAsked->pWitness->name).c_str());

    bool cardDeduced;
    if (pAsked->shown)
    {
        if (!pAsked->cardShown.empty())
        {
            auto itCard = std::_Find_pr(pAsked->pCards.begin(), pAsked->pCards.end(), pAsked->cardShown,
                [](const Card* c, const str& s) { return (c->name == s); });
            if (itCard == pAsked->pCards.end())
                throw std::exception("Card shown not found amongst cards");

            cardDeduced = it->processHas(*itCard);
        }
        else
            cardDeduced = it->processHasEither(pAsked->pCards);
    }
    else
        cardDeduced = it->processDoesntHave(pAsked->pCards) || exteriorChecks();

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
        auto it = std::find(m_analyses.begin(), m_analyses.end(), pGuessed->pDetective);
        if (it == m_analyses.end())
            throw std::exception((str("Failed to find analysis for player ") + pGuessed->pDetective->name).c_str());

        ++m_numStages;
        it->out = true;
        for (Analysis& analysis : m_analyses)
            analysis.processGuessedWrong(it->collectDoesntHave());
    }
}

void Controller::continueDeducing()
{
    // This could loop a few times. One deduction could lead to another and so on
    bool cardDeduced = true;
    while (cardDeduced)
    {
        cardDeduced = false;
        for (auto aIt = m_analyses.begin(); aIt < m_analyses.end(); ++aIt)
            cardDeduced |= aIt->recheckCards();

        cardDeduced |= exteriorChecks();
    }
}

bool Controller::exteriorChecks()
{
    bool cardDeduced = false;
    for (std::vector<Card>& category : m_cards)
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

        for (Card* pCard : unknownCards)
        {
            std::vector<Analysis*> analysesCouldHaveCard;
            for (Analysis& analysis : m_analyses)
            {
                if (analysis.couldHaveCard(pCard))
                    analysesCouldHaveCard.push_back(&analysis);
            }

            switch (analysesCouldHaveCard.size())
            {
            case 0:
                if (guiltyKnown)
                    throw contradiction((pCard->name + str(" has been convicted but another card in the same category was already covicted")).c_str());

                // Everyone has said no to this card so it must be the murder card
                cardDeduced = true;
                guiltyKnown = true;
                unknownCards.front()->conviction = Conviction::GUILTY;
                break;
            case 1:
                // We know which card is guilty and everyone else has said no to this card
                if (guiltyKnown)
                    cardDeduced = analysesCouldHaveCard.front()->processHas(pCard);
            }
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
                guiltyKnown = true;
                unknownCards.front()->conviction = Conviction::GUILTY;
            }
        }
    }

    return cardDeduced;
}
