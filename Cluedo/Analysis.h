#pragma once

#include "Stage.h"

struct Analysis
{
    const Player* pPlayer;
    std::vector<Stage> stages;

    Analysis(const Player* pPlayer) :
        pPlayer(pPlayer)
    {
        stages.emplace_back(pPlayer);
    }

    bool processHas(Card* pCard)
    {
        return stages.back().processHas(pCard);
    }
        
    bool processDoesntHave(const std::vector<Card*>& pCards)
    {
        // Player doesn't have a card, they must never have had that card so this applies to every stage
        bool result = false;
        for (Stage& stage : stages)
            result |= processDoesntHave(pCards);

        return result;
    }

    bool processHasEither(const std::vector<Card*>& pCards)
    {
        // Player now has one of these card, may not have had it before so this only applies to the current stage
        return stages.back().processHasEither(pCards);
    }

    bool recheckCards()
    {
        // recheck cards at each stage
        bool result = false;
        for (Stage& stage : stages)
            result |= recheckCards();

        return result;
    }

    void processGuessedWrong(const std::set<Card*>& guesserDoesntHave)
    {
        std::set<Card*> newDoesntHave;
        std::set_intersection(
            guesserDoesntHave.begin(),
            guesserDoesntHave.end(),
            stages.back().doesntHave.begin(),
            stages.back().doesntHave.end(),
            std::inserter(newDoesntHave, newDoesntHave.begin())
        );

        stages.emplace_back(pPlayer, newDoesntHave);
    }

    str to_str() const
    {
        auto has = collectHas();
        str info = pPlayer->name + str("\n\thas: ");
        for (const Card* pCard : has)
            info += pCard->nickname + str(", ");
        if (!has.empty())
            info.resize(info.size() - 2);

        auto hasEither = collectHasEither();
        info += str("\n\thas either: ");
        for (const std::vector<Card*>& pCards : hasEither)
        {
            for (const Card* pCard : pCards)
                info += pCard->nickname + str("/");

            info.resize(info.size() - 1);
            info += ", ";
        }
        if (!hasEither.empty())
            info.resize(info.size() - 2);

        auto doesntHave = collectDoesntHave();
        info += "\n\tdoesn't have: ";
        for (const Card* pCard : doesntHave)
            info += pCard->nickname + str(", ");
        if (!doesntHave.empty())
            info.resize(info.size() - 2);

        return info + "\n";
    }

    bool operator==(const Analysis& a) const
    {
        return pPlayer == a.pPlayer;
    }
    bool operator==(const Player* p) const
    {
        return pPlayer == p;
    }

private:
    const std::set<Card*> collectHas() const
    {
        std::set<Card*> has;
        for (auto it = stages.begin(); it < stages.end(); ++it)
            has.insert(it->has.begin(), it->has.end());

        return has;
    }

    const std::vector<std::vector<Card*>> collectHasEither() const
    {
        std::vector<std::vector<Card*>> hasEither;
        for (auto it = stages.begin(); it < stages.end(); ++it)
            hasEither.insert(hasEither.end(), it->hasEither.begin(), it->hasEither.end());

        return hasEither;
    }

    const std::set<Card*> collectDoesntHave() const
    {
        std::set<Card*> doesntHave;
        for (auto it = stages.begin(); it < stages.end(); ++it)
            doesntHave.insert(it->doesntHave.begin(), it->doesntHave.end());

        return doesntHave;
    }
};
