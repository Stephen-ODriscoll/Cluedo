#pragma once

struct Turn;
struct Player
{
    str name;
    //str character;      // Unused for now.
    uint32_t numCards;
    std::set<Card*> pCardsOwned;

    Player()
    {
        static uint16_t playerCount = 0;
        name = str("Player ") + str(++playerCount);
    }

    void updateName(const str& newName)
    {
        name = newName;
    }

    bool operator!=(const Player& player) const { return name != player.name; }
    bool operator==(const str& n) const { return name == n; }
};
