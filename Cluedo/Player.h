#pragma once

struct CardDetails
{
    size_t numCards = 0;
    std::set<Card*> pCardsOwned;
};

struct Player
{
    str name;
    //str character;      // Unused for now.
    std::vector<CardDetails> stageCardDetails = { CardDetails() };

    Player()
    {
        static size_t playerCount = 0;
        name = str("Player ") + str(++playerCount);
    }

    void updateName(const str& newName)
    {
        name = newName;
    }

    bool operator!=(const Player& player) const { return name != player.name; }
    bool operator==(const str& n) const { return name == n; }
};
