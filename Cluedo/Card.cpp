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
        throw contradiction(str("Deduced that ") + name + str(" is guilty but this card is already innocent"));

    case Conviction::UNKNOWN:
        if (g_categories[categoryIndex].pPossibleGuilty.find(this) == g_categories[categoryIndex].pPossibleGuilty.end())
            throw std::exception((str("Guilty card ") + name + str(" not found in list of possible guilty cards")).c_str());

        conviction = Conviction::GUILTY;
        g_progressReport += name + str(" has been marked guilty\n");

        for (Card& card : g_categories[categoryIndex].cards)
        {
            if (&card != this)
                card.processInnocent();
        }

        for (Player& player : g_players)
        {
            player.processDoesntHave({ this }, player.stages.size() - 1);
        }
    }
}

void Card::processInnocent()
{
    switch (conviction)
    {
    case Conviction::GUILTY:
        throw contradiction(str("Deduced that ") + name + str(" is innocent but this card is already guilty"));

    case Conviction::UNKNOWN:
        if (!g_categories[categoryIndex].pPossibleGuilty.erase(this))
            throw std::exception((str("Innocent card ") + name + str(" not found in list of possible guilty cards")).c_str());

        conviction = Conviction::INNOCENT;
        g_progressReport += name + str(" has been marked innocent\n");

        recheckLocation();
        g_categories[categoryIndex].recheckGuilty();
    }
}

/*
* Always call processHas, this call will be handled
* 
* If a player owns this card, then the only players who can have owned it earlier are this player and any players who are now out.
*/
void Card::processBelongsTo(Player* pPlayer, const size_t stageIndex)
{
    if (ownedBy(pPlayer, stageIndex))
        return;

    processInnocent();
    if (!couldBelongTo(pPlayer, stageIndex))
        throw contradiction(name + str(" can't be owned by ") + pPlayer->name + str(" (Stage " + stageIndex + str(")")));

    for (size_t i = stageIndex; pPlayer->isIn(i); ++i)
    {
        stages[i].pOwner = pPlayer;

        for (Player* pPlayerLeft : g_pPlayersLeft)
        {
            if (pPlayerLeft != pPlayer)
                pPlayerLeft->processDoesntHave({ this }, i);
        }
    }
}

/*
* Always call processDoesntHave, this call will be handled
* 
* If this card doesn't belong to a player, they can't have had it earlier.
* Once a player gets a card they hold it until they're out.
*/
void Card::processDoesntBelongTo(Player* pPlayer, const size_t stageIndex)
{
    if (!couldBelongTo(pPlayer, stageIndex))
        return;

    if (ownedBy(pPlayer, stageIndex))
        throw contradiction(str("Previous info says ") + pPlayer->name + str(" has ") + name);

    for (size_t i = stageIndex + 1; i != 0;)
    {
        if (!stages[--i].pPossibleOwners.erase(pPlayer))
            break;
    }
    
    recheckLocation();
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
void Card::recheckLocation()
{
    for (size_t i = 0; i != stages.size(); ++i)
    {
        if (locationKnown(i))
            continue;

        switch (stages[i].pPossibleOwners.size())
        {
        case 0:
            processGuilty();
            break;

        case 1:
            if (conviction == Conviction::INNOCENT)
                (*stages[i].pPossibleOwners.begin())->processHas(this, i);
        }
    }
}

bool Card::ownerKnown(const size_t stageIndex) const { return stages[stageIndex].pOwner; }
bool Card::locationKnown(const size_t stageIndex) const { return ownerKnown(stageIndex) || conviction == Conviction::GUILTY; }
bool Card::couldBelongTo(Player* pPlayer, const size_t stageIndex) const { return (stages[stageIndex].pPossibleOwners.find(pPlayer) != stages[stageIndex].pPossibleOwners.end()); }
bool Card::ownedBy(const Player* pPlayer, const size_t stageIndex) const { return stages[stageIndex].pOwner == pPlayer; }

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
* If All other cards have been ruled out the only card left must be guilty
*/
void Category::recheckGuilty()
{
    switch (pPossibleGuilty.size())
    {
    case 0:
        throw contradiction(str("Ruled out all cards in category starting with ") + cards.front().name);

    case 1:
        (*pPossibleGuilty.begin())->processGuilty();
    }
}
