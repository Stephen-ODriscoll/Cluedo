#pragma once

#include <set>

struct Turn;
struct Player
{
    bool out;
    str name;
    str character;      // Unused for now.
    uint32_t numCards;
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
        name = str("P") + str(playerCount++);

        str newName = readStr(str("Enter ") + name + str("'s name or ") + BLANK);
        if (newName != BLANK)
        {
            name = newName;
        }
    }

    void updateNumCards()
    {
        numCards = (g_cardsEvenlyDistributed) ?
            (int)g_numCards / g_numPlayers :
            readInt(str("How many cards does player ") + name + str(" have?"), 0, (int)g_numCards);
    }

    void updateCardsOwned()
    {
        while (pCardsOwned.size() < numCards)
        {
            Card* pCard = readCard(pCardsOwned.size() + 1);

            if (pCard == nullptr)
                break;

            pCardsOwned.insert(pCard);      // I own this card
            pCard->pOwner = this;           // This card is owned by me
        }

        clear();
    }

    bool operator!=(const Player& player)
    {
        return name != player.name;
    }
};

std::vector<Player> g_players;