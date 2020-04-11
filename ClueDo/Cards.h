#pragma once

#include "pch.h"

struct Player;
struct Card
{
    str name;
    uint32_t category;
    Player* pOwner = nullptr;

    Card(const str& name, const uint32_t category) :
        name(name),
        category(category)
    { }

    bool locationKnown() { return pOwner == nullptr; }

    bool operator<(const Card& card) { return name < card.name; }
};

std::vector<std::vector<Card>> g_cards(NUM_CATEGORIES, std::vector<Card>());


size_t totalCards()
{
    size_t total = 0;
    for (std::vector<Card>& category : g_cards)
        total += category.size();

    return total;
}


Card* readCard(size_t number)
{
    std::vector<Card*> possibleCards;
    while (possibleCards.empty())
    {
        str read(readStr(str("Enter card ") + str(number) + str(" or ") + str(BLANK)));
        for (std::vector<Card>& category : g_cards)
            for (Card& card : category)
                if (caseInsensitiveFind(card.name, read))
                    possibleCards.push_back(&card);

        if (read == BLANK)
            return nullptr;
    }

    int index = 0;
    if (possibleCards.size() != 1)
    {
        str message = "Which card?\n";
        for (uint32_t i = 0U; i < possibleCards.size(); ++i)
            message += str(i + 1) + ".) " + possibleCards[i]->name + "\n";

        index = readInt(message, 1, (int)possibleCards.size()) - 1;
    }

    return possibleCards[index];
}
