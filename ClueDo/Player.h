#pragma once

#include <set>

struct Turn;
struct Player
{
    bool out;
    std::string name;
    uint32_t numCards;
    std::string character;
    std::vector<Turn*> pTurns;
    std::set<Card*> pCardsOwned;

    Player() :
        out(false)
    {
        updateName();
        updateNumCards();
        updateCardsOwned();
    }

    void updateName()
    {
        static uint16_t playerCount = 1;
        this->name = std::string("P") + std::to_string(playerCount++);

        std::string name = readString(std::string("Enter ") + this->name + std::string("'s name or ") + BLANK);
        if (name != BLANK)
        {
            this->name = name;
        }
    }

    void updateNumCards()
    {
        numCards = (g_cardsEvenlyDistributed) ?
            (int)g_cards.size() / g_numPlayers :
            readInt(std::string("How many cards does player ") + name + std::string(" have?"), 0, (int)g_cards.size());
    }

    void updateCardsOwned()
    {
        while (pCardsOwned.size() < numCards)
        {
            Card* pCard = readCard();

            if (pCard == nullptr)
            {
                break;
            }

            pCardsOwned.insert(pCard);
        }

        clear();
    }
};

std::vector<Player> g_players;
