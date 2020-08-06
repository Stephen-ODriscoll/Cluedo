#pragma once

#include "Stage.h"

struct Analysis
{
    bool out;
    const Player* pPlayer;
    std::vector<Stage> stages;

    Analysis(const Player* pPlayer) :
        pPlayer(pPlayer),
        out(false)
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
        if (out) return false;

        // recheck cards at each stage
        bool result = false;
        for (Stage& stage : stages)
            result |= recheckCards();

        return result;
    }

    void processGuessedWrong(const std::set<Card*>& guesserDoesntHave)
    {
        if (out) return;

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

    bool couldHaveCard(const Card* pCard)
    {
        const std::set<Card*>& doesntHave = collectDoesntHave();
        return (std::find(doesntHave.begin(), doesntHave.end(), pCard) == doesntHave.end());
    }

    str to_str(size_t stageNum) const
    {
        if (stages.size() < stageNum)
            return "";

        auto has = collectHas(stageNum);
        str info = pPlayer->name + str("\n\thas: ");
        for (const Card* pCard : has)
            info += pCard->nickname + str(", ");
        if (!has.empty())
            info.resize(info.size() - 2);

        auto hasEither = collectHasEither(stageNum);
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

        auto doesntHave = collectDoesntHave(stageNum);
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
    const std::set<Card*> collectHas(const size_t stageNum) const
    {
        std::set<Card*> has;
        const auto itEnd = stages.begin() + stageNum;
        for (auto it = stages.begin(); it < itEnd; ++it)
            has.insert(it->has.begin(), it->has.end());

        return has;
    }

    const std::vector<std::vector<Card*>> collectHasEither(const size_t stageNum) const
    {
        std::vector<std::vector<Card*>> hasEither;
        const auto itEnd = stages.begin() + stageNum;
        for (auto it = stages.begin(); it < itEnd; ++it)
            hasEither.insert(hasEither.end(), it->hasEither.begin(), it->hasEither.end());

        return hasEither;
    }

    const std::set<Card*> collectDoesntHave(const size_t stageNum) const
    {
        return stages[stageNum - 1].doesntHave;
    }

public:
    const std::set<Card*>& collectDoesntHave() const
    {
        return stages.back().doesntHave;
    }
};
