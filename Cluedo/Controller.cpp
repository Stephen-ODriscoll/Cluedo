#include "stdafx.h"
#include "Controller.h"

Controller::Controller(Cluedo* pGUI, fs::path inputFile) :
    m_mode(Mode::NONE),
    m_gameOver(false),
    m_numPlayers(-1),
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

            load.close();
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
    m_numPlayers = numPlayers;

    for (int i = 0; i < numPlayers; ++i)
        m_players.push_back(Player());

    analysesSetup();
    m_pGUI->startGame();
}

void Controller::analysesSetup()
{
    m_analyses.clear();
    for (Player& player : m_players)
        m_analyses.push_back(&player);
}

void Controller::analyseTurn(std::shared_ptr<const Turn> pTurn)
{
    m_pTurns.push_back(pTurn);

    try
    {
        switch (pTurn->action)
        {
        case Action::MISSED:
            break;

        case Action::ASKED:
            analyseAsked(std::static_pointer_cast<const Asked>(pTurn));
            break;

        case Action::GUESSED:
            analyseGuessed(std::static_pointer_cast<const Guessed>(pTurn));
            break;
        }
    }
    catch (const contradiction& ex) { m_pGUI->critical("Contraditory Info Given", ex.what()); }
    catch (const std::exception& ex) { m_pGUI->critical("Exception Occured", ex.what()); }

    m_pGUI->game()->updateStatus();
    m_pGUI->game()->rotateTurn();
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
        cardDeduced = it->processDoesntHave(pAsked->pCards);

    // This could loop a few times. One deduction could lead to another and so on
    while (cardDeduced)
    {
        cardDeduced = false;
        for (auto aIt = m_analyses.begin(); aIt < m_analyses.end(); ++aIt)
            cardDeduced |= aIt->recheckCards();

        cardDeduced |= exteriorChecks();
    }
}

void Controller::analyseGuessed(std::shared_ptr<const Guessed> pGuessed)
{
    if (pGuessed->correct)
    {
        // Game Over
    }
    else
    {
        auto it = std::find(m_analyses.begin(), m_analyses.end(), pGuessed->pDetective);
        if (it == m_analyses.end())
            throw std::exception((str("Failed to find analysis for player ") + pGuessed->pDetective->name).c_str());

        it->out = true;
        for (Analysis& analysis : m_analyses)
            analysis.processGuessedWrong(it->collectDoesntHave());
    }
}

void Controller::reAnalyseTurns(std::shared_ptr<const Turn> oldTurn, std::shared_ptr<const Turn> newTurn)
{
    auto it = std::find(m_pTurns.begin(), m_pTurns.end(), oldTurn);
    if (it == m_pTurns.end())
    {
        m_pGUI->critical("Exception Occured", "Failed to find old turn in turns vector");
        return;
    }

    *it = newTurn;      // Replace this turn with the updated one

    // Start our analysis again
    analysesSetup();
    for (std::shared_ptr<const Turn> pTurn : m_pTurns)
        analyseTurn(pTurn);
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
            if (card.conviction == Conviction::UNKNOWN)
                unknownCards.push_back(&card);
            else if (card.conviction == Conviction::GUILTY)
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
                analysesCouldHaveCard.front()->processHas(pCard);
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

bool Controller::rename(const str& oldName, const str& newName)
{
    auto it = std::find(m_players.begin(), m_players.end(), oldName);
    if (it == m_players.end())
    {
        m_pGUI->critical("Exception", "Failed to find corresponding player");
        return false;
    }

    it->updateName(newName);
    m_pGUI->game()->updateStatus();
    return true;
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
