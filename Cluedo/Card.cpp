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

bool Card::processGuilty()
{
    switch (conviction)
    {
    case Conviction::GUILTY:
        return false;

    case Conviction::INNOCENT:
        throw contradiction((str("Deduced that ") + name + str(" is guilty but this card is already innocent")).c_str());

    case Conviction::UNKNOWN:
        for (Card& card : g_categories[categoryIndex].cards)
        {
            if (&card != this)
                card.processInnocent();
        }

        conviction = Conviction::GUILTY;
        g_progressReport += name + str(" has been convicted\n");
    }

    return true;
}

bool Card::processInnocent()
{
    switch (conviction)
    {
    case Conviction::GUILTY:
        throw contradiction((str("Deduced that ") + name + str(" is innocent but this card is already guilty")).c_str());

    case Conviction::INNOCENT:
        return false;

    case Conviction::UNKNOWN:
        auto it = g_categories[categoryIndex].pPossibleGuilty.find(this);
        if (it == g_categories[categoryIndex].pPossibleGuilty.end())
            throw std::exception((str("Previously unknown card ") + name + str(" not found in list of possible guilty cards")).c_str());

        conviction = Conviction::INNOCENT;
        g_categories[categoryIndex].pPossibleGuilty.erase(it);
        g_progressReport += name + str(" has been marked innocent\n");
    }

    return true;
}

/*
* If a player owns this card, then the only players who can have owned it earlier are this player and any players who are now out.
*/
bool Card::processBelongsTo(Player* pPlayer, const size_t stageIndex)
{
    if (ownedBy(pPlayer, stageIndex))
        return false;

    processInnocent();
    if (!couldBelongTo(pPlayer, stageIndex))
        throw contradiction(name + str(" can't be owned by ") + pPlayer->name + str(" (Stage " + stageIndex + str(")")));
    
    stages[stageIndex].pOwner = pPlayer;
    stages[stageIndex].pPossibleOwners = { pPlayer };

    std::set<Player*> pPossibleOwners = { pPlayer };
    for (size_t i = stageIndex; i != 0;)
    {
        pPossibleOwners.insert(g_pPlayersOut[--i]);

        std::set<Player*> pImpossibleOwners;
        std::set_difference(
            stages[i].pPossibleOwners.begin(),
            stages[i].pPossibleOwners.end(),
            pPossibleOwners.begin(),
            pPossibleOwners.end(),
            std::inserter(pImpossibleOwners, pImpossibleOwners.begin()));

        if (pImpossibleOwners.empty())
            break;

        for (Player* pImpossibleOwner : pImpossibleOwners)
            pImpossibleOwner->processDoesntHave({ this }, i);
    }

    return true;
}

/*
* If this card doesn't belong to a player, they can't have had it earlier.
* Once a player gets a card they hold it until they're out.
*/
bool Card::processDoesntBelongTo(Player* pPlayer, const size_t stageIndex)
{
    if (ownedBy(pPlayer, stageIndex))
        throw contradiction((str("Previous info says ") + pPlayer->name + str(" has ") + name).c_str());

    for (size_t i = stageIndex + 1; i != 0;)
        stages[--i].pPossibleOwners.erase(pPlayer);
    
    return recheck();
}

/*
* If the player that's out couldn't have had this card then our info doesn't change otherwise anyone left can have this card now.
*/
void Card::processGuessedWrong(Player* pGuesser)
{
    if (couldBelongTo(pGuesser, stages.size() - 1))
        stages.push_back(stages.back());
    else
        stages.push_back(g_pPlayersLeft);
}

/*
* At every stage check if the card can only be guilty or only be owned by one person.
*/
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
            cardDeduced |= processGuilty();
            break;

        case 1:
            if (isInnocent())
                cardDeduced |= (*stages[i].pPossibleOwners.begin())->processHas(this, i);
        }
    }

    return cardDeduced;
}

/*
* Keeping a list of all possible owners makes this check a lot more efficient.
*/
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

Category::Category(const std::vector<Card>& cards) :
    cards(cards)
{
    for (Card& card : this->cards)
        pPossibleGuilty.insert(&card);
}

void Category::reset()
{
    for (Card& card : cards)
    {
        card.reset();
        pPossibleGuilty.insert(&card);
    }
}

/*
* If All other cards have been ruled ou the only card left must be guilty
*/
bool Category::recheck()
{
    switch (pPossibleGuilty.size())
    {
    case 0:
        throw contradiction((str("Ruled out all cards in category starting with ") + cards.front().name).c_str());

    case 1:
        return (*pPossibleGuilty.begin())->processGuilty();
    }

    return false;
}
