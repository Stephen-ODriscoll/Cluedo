#include "stdafx.h"
#include "Card.h"

CardStage::CardStage(std::vector<Player>& players) :
    pOwner(nullptr)
{
    for (Player& player : players)
        pPossibleOwners.insert(&player);
}

CardStage::CardStage(std::vector<Player*>& pPlayers) :
    pPossibleOwners(pPlayers.begin(), pPlayers.end()),
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

void Card::processGuilty()
{
    switch (conviction)
    {
    case Conviction::INNOCENT:
        throw contradiction((str("Deduced that ") + name + str(" is guilty but this card is already innocent")).c_str());

    case Conviction::UNKNOWN:
        for (Card& card : g_categories[categoryIndex].cards)
        {
            if (&card != this)
                card.processInnocent();
        }
        
        conviction = Conviction::GUILTY;
        g_categories[categoryIndex].guiltyKnown = true;
        g_progressReport += name + str(" has been convicted\n");
        
        // To-do: Handle guilty re-checking
    }
}

void Card::processInnocent()
{
    switch (conviction)
    {
    case Conviction::GUILTY:
        throw contradiction((str("Deduced that ") + name + str(" is innocent but this card is already guilty")).c_str());

    case Conviction::UNKNOWN:
        conviction = Conviction::INNOCENT;
        g_progressReport += name + str(" has been marked innocent\n");

        // To-do: Handle innocent re-checking
    }
}

void Card::processBelongsTo(Player* pPlayer, const size_t stageIndex)
{
    if (locationKnown(stageIndex))
    {
        // We already know that this person owns this card, no new info
        if (ownedBy(pPlayer, stageIndex))
            return;

        throw contradiction((name + str(" can't be owned by ") + pPlayer->name + str(" and ")
            + (isGuilty() ?
                str("be guilty") :
                stages[stageIndex].pOwner->name)
            ).c_str());
    }

    if (!couldBelongTo(pPlayer, stageIndex))
        throw contradiction((name + str(" can't be owned by ") + pPlayer->name + str(" (Stage " + stageIndex + str(")"))).c_str());
        
    conviction = Conviction::INNOCENT;
    stages[stageIndex].pOwner = pPlayer;
    stages[stageIndex].pPossibleOwners = { pPlayer };

    std::set<Player*> pPossibleOwners = { pPlayer };
    for (int64_t i = int64_t(stageIndex) - 1; i != -1; --i)
    {
        pPossibleOwners.insert(g_pPlayersOut[i]);
        for (auto it = pPossibleOwners.begin(); it != pPossibleOwners.end();)
        {
            if (!couldBelongTo(*it, i))
                it = pPossibleOwners.erase(it);
            else
                ++it;
        }

        // Break here if we already know all this
        if (stages[i].pPossibleOwners == pPossibleOwners)
            break;

        std::set<Player*> pImpossibleOwners;
        std::set_difference(
            stages[i].pPossibleOwners.begin(),
            stages[i].pPossibleOwners.end(),
            pPossibleOwners.begin(),
            pPossibleOwners.end(),
            std::inserter(pImpossibleOwners, pImpossibleOwners.begin())
        );

        for (Player* pImpossibleOwner : pImpossibleOwners)
            pImpossibleOwner->processDoesntHave({ this }, i);
    }

    pPlayer->processHas(this, stageIndex);
}

void Card::processDoesntBelongTo(Player* pPlayer, const size_t stageIndex)
{
    // We already know that this person owns this card, no new info
    if (!couldBelongTo(pPlayer, stageIndex))
        return;

    if (ownedBy(pPlayer, stageIndex))
        throw contradiction((str("Previous info says ") + pPlayer->name + str(" has ") + name).c_str());

    /*
    * Needs more thought about where to recheck
    */

    for (size_t i = 0; i != stageIndex + 1; ++i)
    {
        auto it = stages[i].pPossibleOwners.find(pPlayer);
        if (it != stages[i].pPossibleOwners.end())
            stages[i].pPossibleOwners.erase(it);
    }
    
    recheck();
}

void Card::processGuessedWrong(Player* pGuesser)
{
    if (couldBelongTo(pGuesser, stages.size() - 1))
        stages.push_back(g_pPlayersLeft);
    else
        stages.push_back(stages.back());
}

void Card::recheck()
{
    for (size_t i = 0; i != stages.size(); ++i)
    {
        if (locationKnown(i))
            continue;

        switch (stages[i].pPossibleOwners.size())
        {
        case 0:
            // Nobody can have this card so it must be guilty
            processGuilty();
            break;

        case 1:
            // If this card is innocent then we know it's only possible owner is it's owner
            if (isInnocent())
                (*stages[i].pPossibleOwners.begin())->processHas(this, i);
        }
    }
}

bool Card::couldBelongTo(Player* pPlayer, const size_t stageIndex) const
{
    return (stages[stageIndex].pPossibleOwners.find(pPlayer) != stages[stageIndex].pPossibleOwners.end());
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


void Category::reset()
{
    guiltyKnown = false;
    for (Card& card : cards)
        card.reset();
}
