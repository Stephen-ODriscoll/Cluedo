#pragma once

#include "stdafx.h"

struct Turn;
struct Player
{
    bool out;
    str name;
    //str character;      // Unused for now.
    uint32_t numCards;
    std::vector<Turn*> pTurns;
    std::set<Card*> pCardsOwned;

    Player() :
        out(false)
    {
        static uint16_t playerCount = 0;
        name = str("Player ") + str(++playerCount);
    }

    void updateName(str newName)
    {
        name = newName;
    }

    bool operator!=(const Player& player) const { return name != player.name; }
    bool operator==(const str& n) const { return name == n; }
};
