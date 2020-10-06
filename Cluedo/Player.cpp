#include "stdafx.h"
#include "Player.h"

PlayerStage::PlayerStage() { }
PlayerStage::PlayerStage(std::set<Card*> has, std::set<Card*> doesntHave, std::vector<std::vector<Card*>> hasEither) :
    has(has),
    doesntHave(doesntHave),
    hasEither(hasEither) { }

Player::Player() :
    stages({ PlayerStage() })
{
    static size_t playerCount = 0;
    name = str("Player ") + str(++playerCount);
}

void Player::reset()
{
    stages = { PlayerStage() };
}

bool Player::processHas(Card* pCard, const size_t stageIndex)
{
    bool result = false;
    for (size_t i = stageIndex; i != stages.size(); ++i)
    {
        if (!pCard->processBelongsTo(this, stageIndex))
            return result;
        
        stages[i].has.insert(pCard);
        result = true;
    }
    
    return result;
}

bool Player::processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex)
{
    bool result = false;
    for (Card* pCard : pCards)
    {
        pCard->processDoesntBelongTo(this, stageIndex);

        for (size_t i = stageIndex; i != 0; --i)
        {
            if (pCard->locationUnknown(i))
                stages[i].doesntHave.insert(pCard);
        }
    } 
    
    result |= recheck();
    return result;
}

bool Player::processHasEither(const std::vector<Card*>& pCards, const size_t stageIndex)
{
    std::vector<Card*> checkedCards;
    for (Card* pCard : pCards)
    {
        if (couldHaveCard(pCard, stageIndex))
            checkedCards.emplace_back(pCard);
    }

    switch (checkedCards.size())
    {
    case 0:
        throw contradiction((name + str(" can't have any of those cards")).c_str());

    case 1:
        return processHas(checkedCards.front(), stageIndex);

    default:
        stages[stageIndex].hasEither.push_back(checkedCards);      // We don't know for sure which card was shown (yet)
    }

    return false;
}

bool Player::recheck()
{
    bool cardFound = false;
    for (size_t i = 0; i != stages.size(); ++i)
    {
        for (auto it = stages[i].doesntHave.begin(); it != stages[i].doesntHave.end();)
        {
            // If card location is known remove it from doesn't have.
            // We already know they don't have it because someone else does.
            if ((*it)->locationKnown(i))
                it = stages[i].doesntHave.erase(it);
            else
                ++it;
        }

        // Recheck past cards with this new info
        for (auto it1 = stages[i].hasEither.begin(); it1 != stages[i].hasEither.end();)
        {
            for (auto it2 = it1->begin(); it2 != it1->end();)
            {
                if (couldHaveCard(*it2, i))
                    ++it2;
                else
                    it2 = it1->erase(it2);
            }

            switch (it1->size())
            {
            case 0:
                throw contradiction((name + str(" can't have any of the 3 cards they're supposed to")).c_str());

            case 1:
                cardFound = processHas(it1->front(), i);
                it1 = stages[i].hasEither.erase(it1);
                break;

            default:
                ++it1;
            }
        }
    }

    return cardFound;
}

bool Player::processGuessedWrong(Player* pPlayer)
{
    std::set<Card*> newDoesntHave;
    std::set_intersection(
        pPlayer->stages.back().doesntHave.begin(),
        pPlayer->stages.back().doesntHave.end(),
        stages.back().doesntHave.begin(),
        stages.back().doesntHave.end(),
        std::inserter(newDoesntHave, std::next(newDoesntHave.begin()))
    );

    stages.emplace_back(stages.back().has, newDoesntHave, stages.back().hasEither);

    bool result = false;
    if (stagePresets.size() < stages.size())
        stagePresets.emplace_back(stagePresets.back());
    else
    {
        for (Card* pCard : stagePresets[stages.size() - 1].pCardsOwned)
            result |= processHas(pCard, stages.size() - 1);
    }

    return result;
}

bool Player::couldHaveCard(Card* pCard, size_t stageIndex)
{
    // This card could've been shown if this player hasn't said no to it and either we don't know who owns it or they own it

    return (stages[stageIndex].doesntHave.find(pCard) == stages[stageIndex].doesntHave.end() &&
        (pCard->locationUnknown(stageIndex) || pCard->ownedBy(this, stageIndex)));
}

str Player::to_str(size_t stageIndex) const
{
    if (stages.size() <= stageIndex)
        return "";

    const PlayerStage& stage = stages[stageIndex];

    str info = name + str("\n\thas: ");
    for (const Card* pCard : stage.has)
        info += pCard->nickname + str(", ");
    if (!stage.has.empty())
        info.resize(info.size() - 2);

    info += str("\n\thas either: ");
    for (const std::vector<Card*>& pCards : stage.hasEither)
    {
        for (const Card* pCard : pCards)
            info += pCard->nickname + str("/");

        info.resize(info.size() - 1);
        info += ", ";
    }
    if (!stage.hasEither.empty())
        info.resize(info.size() - 2);

    info += "\n\tdoesn't have: ";
    for (const Card* pCard : stage.doesntHave)
        info += pCard->nickname + str(", ");
    if (!stage.doesntHave.empty())
        info.resize(info.size() - 2);

    return info + "\n\n";
}

bool Player::operator!=(const Player& player) const { return name != player.name; }
bool Player::operator==(const str& n) const { return name == n; }
