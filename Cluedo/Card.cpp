#include "stdafx.h"
#include "Card.h"

CardStage::CardStage(std::vector<Player>& players) :
    pOwner(nullptr)
{
    for (Player& player : players)
        pPossibleOwners.insert(&player);
}

CardStage::CardStage(std::set<Player*>& pPlayers) :
    pPossibleOwners(pPlayers),
    pOwner(nullptr) { }

Card::Card(const str& name, const str& nickname, const size_t categoryIndex) :
    name(name),
    nickname(nickname),
    categoryIndex(categoryIndex),
    stages({ CardStage(g_players) }),
    conviction(Conviction::UNKNOWN)
{ }

void Card::reset()
{
    stages = { CardStage(g_players) };
    conviction = Conviction::UNKNOWN;
}

bool Card::processBelongsTo(Player* pPlayer, const size_t stageIndex)
{
    if (locationKnown(stageIndex))
    {
        // We already know that this person owns this card, no new info
        if (ownedBy(pPlayer, stageIndex))
            return false;

        throw contradiction((
            str("Deduced that ") + name + str(" is owned by ") + pPlayer->name
            + str(" but this card is already ")
            + (isGuilty() ?
                str("guilty") :
                str("owned by ")) + stages[stageIndex].pOwner->name
            ).c_str());
    }

    if (stages[stageIndex].pPossibleOwners.find(pPlayer) == stages[stageIndex].pPossibleOwners.end())
        throw contradiction((str("Deduced that ") + name + str(" is owned by ") + pPlayer->name
        + str(" but this scenario has already been ruled out")).c_str());
        
    conviction = Conviction::INNOCENT;
    stages[stageIndex].pOwner = pPlayer;
    stages[stageIndex].pPossibleOwners = { pPlayer };

    std::set<Player*> pPossibleOwners = { pPlayer };
    for (int64_t i = int64_t(stageIndex) - 1; i != -1; --i)
    {
        pPossibleOwners.insert(g_pPlayersOut[i]);
        for (auto it = pPossibleOwners.begin(); it != pPossibleOwners.end();)
        {
            if (stages[i].pPossibleOwners.find(*it) == stages[i].pPossibleOwners.end() || !(*it)->couldHaveCard(this, i))
                it = pPossibleOwners.erase(it);
            else
                ++it;
        }

        stages[i].pPossibleOwners = pPossibleOwners;
    }

    return true;
}

bool Card::processDoesntBelongTo(Player* pPlayer, const size_t stageIndex)
{
    if (ownedBy(pPlayer, stageIndex))
        throw contradiction((str("Previous info says ") + pPlayer->name + str(" has ") + name).c_str());

    for (size_t i = stageIndex; i != 0; --i)
        stages[i].pPossibleOwners.erase(pPlayer);
    
    return true;
}

void Card::processGuessedWrong(Player* pPlayer)
{
    // If the player that's out couldn't have had this card then our info doesn't change
    if (stages.back().pPossibleOwners.find(pPlayer) == stages.back().pPossibleOwners.end())
        stages.push_back(stages.back());
    else
        stages.push_back(g_pPlayersLeft);   // Otherwise anyone left can have it now
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
