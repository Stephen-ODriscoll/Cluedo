#pragma once

struct Player;
struct Card
{
    std::string name;
    bool convicted = false;
    Player* pOwner = nullptr;

    Card(const std::string& name) :
        name(name)
    { }

    bool locationKnown() { return pOwner == nullptr; }

    bool operator<(const Card& card) { return name < card.name; }
};

std::vector<std::vector<Card>> g_cards;


Card* readCard()
{
    std::vector<Card*> possibleCards;
    while (possibleCards.empty())
    {
        std::string read = readString(std::string("Enter card name or ") + std::string(BLANK));
        for (std::vector<Card>& category : g_cards)
        {
            for (Card& card : category)
            {
                if (card.name.find(read) != std::string::npos)
                {
                    possibleCards.push_back(&card);
                }
            }
        }

        if (read == BLANK)
        {
            return nullptr;
        }
    }

    int index = 0;
    if (possibleCards.size() != 1)
    {
        std::string message = "Which card?\n";
        for (uint32_t i = 0U; i < possibleCards.size(); ++i)
        {
            message += std::to_string(i + 1) + ".) " + possibleCards[i]->name + "\n";
        }

        index = readInt(message, 1, (int)possibleCards.size()) - 1;
    }

    return possibleCards[index];
}
