#include "stdafx.h"
#include "Card.h"

CardStage::CardStage(std::vector<Player>& players) :
    pOwner(nullptr)
{
    for (Player& player : players)
        pPossibleOwners.insert(&player);
}

Card::Card(const str& name, const str& nickname) :
    name(name),
    nickname(nickname),
    stages({ CardStage(g_players) }),
    conviction(Conviction::UNKNOWN)
{ }

void Card::reset()
{
    stages = { CardStage(g_players) };
    conviction = Conviction::UNKNOWN;
}

bool Card::isGuilty() const { return conviction == Conviction::GUILTY; }
bool Card::isUnknown() const { return conviction == Conviction::UNKNOWN; }
bool Card::isInnocent() const { return conviction == Conviction::INNOCENT; }

bool Card::ownerKnown(const size_t stageIndex) const { return stages[stageIndex].pOwner; }
bool Card::ownerUnknown(const size_t stageIndex) const { return !stages[stageIndex].pOwner; }
bool Card::ownedBy(const Player* pPlayer, const size_t stageIndex) const { return stages[stageIndex].pOwner == pPlayer; }
bool Card::locationKnown(const size_t stageIndex) const { return ownerKnown(stageIndex) || isGuilty(); }
bool Card::locationUnknown(const size_t stageIndex) const { return !locationKnown(stageIndex); }

bool Card::operator<(const Card& card) const { return name < card.name; }
bool Card::operator==(const str& n) const { return name == n; }
