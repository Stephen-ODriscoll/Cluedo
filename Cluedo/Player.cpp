#include "stdafx.h"
#include "Player.h"

StagePreset::StagePreset() :
    numCards(0) { }
StagePreset::StagePreset(const std::set<Card*> pCardsOwned) :
    numCards(0),
    pCardsOwned(pCardsOwned) { }
StagePreset::StagePreset(const size_t numCards, const std::set<Card*> pCardsOwned) :
    numCards(numCards),
    pCardsOwned(pCardsOwned) { }

bool StagePreset::isNumCardsKnown() const { return !!numCards; }
bool StagePreset::operator==(const StagePreset& stagePreset) const { return (numCards == stagePreset.numCards && pCardsOwned == stagePreset.pCardsOwned); }

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

bool Player::reset()
{
    stages = { PlayerStage() };

    bool result = false;
    for (Card* pCard : presets.front().pCardsOwned)
        result |= processHas(pCard, 0);

    return result;
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

    if (result)
        g_progressReport += name + str(" owns ") + pCard->name + str(" (Stage ") + str(stageIndex + 1) + str(")\n");
    
    return result;
}

bool Player::processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex)
{
    bool result = false;
    for (Card* pCard : pCards)
    {
        result |= pCard->processDoesntBelongTo(this, stageIndex);

        for (size_t i = 0; i != stageIndex + 1; ++i)
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
                cardFound |= processHas(it1->front(), i);
                it1 = stages[i].hasEither.erase(it1);
                break;

            default:
                ++it1;
            }
        }
    }

    return cardFound;
}

bool Player::processGuessedWrong(Player* pPlayer, int cardsReceived)
{
    bool result = false;
    if (presets.size() <= stages.size())
    {
        StagePreset& preset = presets.back();
        if (cardsReceived == -1)
            presets.emplace_back(0, preset.pCardsOwned);
        else
            presets.emplace_back(preset.numCards + cardsReceived, preset.pCardsOwned);
    }
    else
    {
        for (Card* pCard : presets[stages.size() - 1].pCardsOwned)
            result |= processHas(pCard, stages.size() - 1);
    }

    if (presets[stages.size()].numCards)
    {
        std::set<Card*> newDoesntHave;
        std::set_intersection(
            pPlayer->stages.back().doesntHave.begin(),
            pPlayer->stages.back().doesntHave.end(),
            stages.back().doesntHave.begin(),
            stages.back().doesntHave.end(),
            std::inserter(newDoesntHave, newDoesntHave.begin())
        );

        stages.emplace_back(stages.back().has, newDoesntHave, stages.back().hasEither);
    }
    else
    {
        stages.emplace_back(stages.back());
    }

    return result;
}

bool Player::allCardsKnown(size_t stageIndex) const
{
    return (presets[stageIndex].isNumCardsKnown() && stages[stageIndex].has.size() == presets[stageIndex].numCards);
}

bool Player::couldHaveCard(Card* pCard, size_t stageIndex) const
{
    return pCard->ownedBy(this, stageIndex) ||
        (pCard->locationUnknown(stageIndex) && !allCardsKnown(stageIndex) &&
        (stages[stageIndex].doesntHave.find(pCard) == stages[stageIndex].doesntHave.end()));
}

str Player::to_str(size_t stageIndex) const
{
    if (stages.size() <= stageIndex)
        return "";

    const PlayerStage& stage = stages[stageIndex];

    return name +
        str("\n\thas: ") + str(stage.has.begin(), stage.has.end(), [](Card* pCard) { return pCard->nickname; }) + str(allCardsKnown(stageIndex) ? " (All)" : "") +
        str("\n\thas either: ") + str(stage.hasEither.begin(), stage.hasEither.end(), [](std::vector<Card*> pCards)
            { return str(pCards.begin(), pCards.end(), [](Card* pCard) { return pCard->nickname; }, "/"); }) +
        str("\n\tdoesn't have: ") + str(stage.doesntHave.begin(), stage.doesntHave.end(), [](Card* pCard) { return pCard->nickname; }) +
        "\n\n";
}

bool Player::operator!=(const Player& player) const { return name != player.name; }
bool Player::operator==(const str& n) const { return name == n; }
