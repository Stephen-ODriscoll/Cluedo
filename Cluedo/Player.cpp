#include "stdafx.h"
#include "Player.h"

PlayerStage::PlayerStage() { }
PlayerStage::PlayerStage(std::set<Card*> has, std::set<Card*> doesntHave, std::vector<std::vector<Card*>> hasEither) :
    has(has),
    doesntHave(doesntHave),
    hasEither(hasEither) { }

Player::Player() :
    out(false),
    stages({ PlayerStage() })
{
    static size_t playerCount = 0;
    name = str("Player ") + str(++playerCount);
}

void Player::reset()
{
    out = false;
    stages = { PlayerStage() };
}

bool Player::processHas(Card* pCard, const size_t stageIndex)
{
    if (pCard->locationKnown(stageIndex))
    {
        // We already know that this person owns this card, no new info
        if (pCard->ownedBy(this, stageIndex))
            return false;

        throw contradiction((
            str("Deduced that ") + pCard->name + str(" is owned by ") + name
            + str(" but this card is already ")
            + (pCard->isGuilty() ?
                str("guilty") :
                str("owned by ")) + pCard->stages[stageIndex].pOwner->name
            ).c_str());
    }

    // We've found a card and have new info to go on
    pCard->conviction = Conviction::INNOCENT;
    pCard->stages[stageIndex].pOwner = this;

    for (size_t i = stageIndex; i != stages.size(); ++i)
        stages[i].has.insert(pCard);

    return true;
}

bool Player::processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex)
{
    bool result = false;
    for (size_t i = stageIndex; i != 0; --i)
    {
        for (Card* pCard : pCards)
        {
            if (pCard->ownedBy(this, i))
                throw contradiction((str("Previous info says ") + name + str(" has ") + pCard->name).c_str());

            if (pCard->locationUnknown(i))
                stages[i].doesntHave.insert(pCard);
        }

        result |= recheckCards(stageIndex);
    }

    return result;
}

bool Player::processHasEither(const std::vector<Card*>& pCards, const size_t stageIndex)
{
    std::vector<Card*> checkedCards;
    for (Card* pCard : pCards)
    {
        // This card could've been shown if this player hasn't said no to it and
        // either we don't know who owns it or they own it
        if (stages[stageIndex].doesntHave.find(pCard) == stages[stageIndex].doesntHave.end() &&
            (pCard->locationUnknown(stageIndex) || pCard->ownedBy(this, stageIndex)))
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

bool Player::recheckCards(const size_t stageIndex)
{
    for (auto it = stages[stageIndex].doesntHave.begin(); it != stages[stageIndex].doesntHave.end();)
    {
        // If card location is known remove it from doesn't have.
        // We already know they don't have it because someone else does.
        if ((*it)->locationKnown(stageIndex))
            it = stages[stageIndex].doesntHave.erase(it);
        else
            ++it;
    }

    // Recheck past cards with this new info
    bool cardFound = false;
    for (auto it1 = stages[stageIndex].hasEither.begin(); it1 != stages[stageIndex].hasEither.end();)
    {
        for (auto it2 = it1->begin(); it2 != it1->end();)
        {
            // This card could've been shown if this player hasn't said no to it and
            // either we don't know who owns it or they own it
            if (stages[stageIndex].doesntHave.find(*it2) == stages[stageIndex].doesntHave.end() &&
                ((*it2)->locationUnknown(stageIndex) || (*it2)->ownedBy(this, stageIndex)))
                ++it2;
            else
                it2 = it1->erase(it2);
        }

        switch (it1->size())
        {
        case 0:
            throw contradiction((name + str(" can't have any of the 3 cards they're supposed to")).c_str());

        case 1:
            cardFound = processHas(it1->front(), stageIndex);
            it1 = stages[stageIndex].hasEither.erase(it1);
            break;

        default:
            ++it1;
        }
    }

    return cardFound;
}

bool Player::processGuessedWrong(const std::set<Card*>& guesserDoesntHave)
{
    if (out) return false;

    bool result = false;
    std::set<Card*> newDoesntHave;
    std::set_intersection(
        guesserDoesntHave.begin(),
        guesserDoesntHave.end(),
        stages.back().doesntHave.begin(),
        stages.back().doesntHave.end(),
        std::inserter(newDoesntHave, newDoesntHave.begin())
    );

    stages.emplace_back(stages.back().has, newDoesntHave, stages.back().hasEither);

    if (stagePresets.size() < stages.size())
        stagePresets.emplace_back(stagePresets.back());
    else
    {
        for (Card* pCard : stagePresets[stages.size() - 1].pCardsOwned)
            result |= processHas(pCard, stages.size() - 1);
    }

    return result;
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
