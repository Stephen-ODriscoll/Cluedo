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
                m_cards[category].push_back(Card(splits[0], splits[1], category));
            }

            if (m_cards.size() != NUM_CATEGORIES)
                throw std::invalid_argument(str("Not enough categories. There should be ") + str(NUM_CATEGORIES));

            load.close();
            break;          // This is how we complete the constructor
        }
        catch (const std::invalid_argument& ex)
        {
            m_cards.clear();

            inputFile = fs::path(m_pGUI->openCluedoTextFile(ex.what()).string());
            if (inputFile.empty())
                exit(EXIT_SUCCESS);     // User clicked cancel
        }
    }
}

Controller::~Controller()
{
    for (Turn* pTurn : m_pTurns)
        delete pTurn;

    m_pTurns.clear();
}

void Controller::startGame(Mode mode, int numPlayers)
{
    m_mode = mode;
    m_numPlayers = numPlayers;

    for (int i = 0; i < numPlayers; ++i)
        m_players.push_back(Player());

    m_pGUI->startGame();
}

void Controller::analysisSetup()
{
    for (Player& player : m_players)
        m_analysis.push_back(&player);

    for (std::vector<Card>& category : m_cards)
    {
        for (Card& card : category)
        {
            if (card.pOwner == nullptr)         // Unowned, we suspect it
                m_pPossibleCards.insert(&card);
            else
            {
                auto it = std::find(m_analysis.begin(), m_analysis.end(), card.pOwner);
                if (it != m_analysis.end())
                    it->has.insert(&card);
            }
        }
    }
}

void Controller::analyseMissed(Missed* pMissed)
{
    m_pTurns.push_back(pMissed);
}

void Controller::analyseAsked(Asked* pAsked)
{
    m_pTurns.push_back(pAsked);

    auto it = std::find(m_analysis.begin(), m_analysis.end(), *pAsked);
    if (it != m_analysis.end())
    {
        bool cardDeduced;
        if (pAsked->shown)
            cardDeduced = it->processHasEither(pAsked->pCards, m_pPossibleCards);
        else
            cardDeduced = it->processDoesntHave(pAsked->pCards, m_pPossibleCards);

        // This could loop a few times. One deduction could lead to another and so on
        while (cardDeduced)
        {
            cardDeduced = false;
            for (auto aIt = m_analysis.begin(); aIt < m_analysis.end(); ++aIt)
                cardDeduced |= aIt->processPossibleCards(m_pPossibleCards);
        }
    }
}

void Controller::analyseGuessed(Guessed* pGuessed)
{
    m_pTurns.push_back(pGuessed);
}

bool Controller::rename(str oldName, str newName)
{
    auto& it = std::find(m_players.begin(), m_players.end(), oldName);

    if (it == m_players.end())
        return false;

    it->updateName(newName);
    return true;
}

const std::vector<std::vector<Card>>& Controller::cards()
{
    return m_cards;
}

const std::vector<Player>& Controller::players()
{
    return m_players;
}

const std::vector<Turn*>& Controller::turns()
{
    return m_pTurns;
}
