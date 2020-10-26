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

bool Card::processBelongsTo(Player* pPlayer, const size_t stageIndex)
{
    if (locationKnown(stageIndex))
    {
        // We already know that this person owns this card, no new info
        if (ownedBy(pPlayer, stageIndex))
            return false;

        throw contradiction((name + str(" can't be owned by ") + pPlayer->name + str(" and ")
            + (isGuilty() ?
                str("guilty") :
                stages[stageIndex].pOwner->name)
            ).c_str());
    }

    if (stages[stageIndex].pPossibleOwners.find(pPlayer) == stages[stageIndex].pPossibleOwners.end())
        throw contradiction((name + str(" can't be owned by ") + pPlayer->name
        + str(" because this has already been ruled out")).c_str());
        
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

    return true;
}

bool Card::processDoesntBelongTo(Player* pPlayer, const size_t stageIndex)
{
    if (ownedBy(pPlayer, stageIndex))
        throw contradiction((str("Previous info says ") + pPlayer->name + str(" has ") + name).c_str());

    for (size_t i = 0; i != stageIndex + 1; ++i)
        stages[i].pPossibleOwners.erase(pPlayer);
    
    return recheck();
}

bool Card::processGuilty()
{
    switch (conviction)
    {
    case Conviction::GUILTY:
        return false;

    case Conviction::INNOCENT:
        throw contradiction((str("Deduced that ") + name + str(" is guilty but this card is already innocent")).c_str());

    case Conviction::UNKNOWN:
        if (g_categories[categoryIndex].guiltyKnown)
            throw contradiction((name + str(" has been convicted but another card in the same category was already covicted")).c_str());

        conviction = Conviction::GUILTY;
        g_categories[categoryIndex].guiltyKnown = true;
        g_progressReport += name + str(" has been convicted\n");

        for (Card& card : g_categories[categoryIndex].cards)
        {
            if (&card != this)
                card.conviction = Conviction::INNOCENT;
        }
    }

    return true;
}

bool Card::recheck()
{
    bool cardDeduced = false;
    for (size_t i = 0; i != stages.size(); ++i)
    {
        if (locationKnown(i))
            continue;

        switch (stages[i].pPossibleOwners.size())
        {
        case 0:
            // Nobody can have this card so it must be guilty
            cardDeduced |= processGuilty();
            break;

        case 1:
            // If this card is innocent then we know it's only possible owner is it's owner
            if (isInnocent())
                cardDeduced |= (*stages[i].pPossibleOwners.begin())->processHas(this, i);
        }
    }

    return cardDeduced;
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


void Category::reset()
{
    guiltyKnown = false;
    for (Card& card : cards)
        card.reset();
}
