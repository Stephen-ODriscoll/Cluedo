#pragma once

#include "Macros.h"

enum class Conviction
{
    UNKNOWN,
    INNOCENT,
    GUILTY
};

const std::map<Conviction, str> convictionStrings =
{
    { Conviction::UNKNOWN,  "" },
    { Conviction::INNOCENT, "Innocent" },
    { Conviction::GUILTY,   "Guilty" }
};

struct Player;
struct CardStage
{
    Player* pOwner;
    std::set<Player*> pPossibleOwners;

    CardStage(std::vector<Player>& players);
};

struct Card
{
    const str name, nickname;

    std::vector<CardStage> stages;
    enum class Conviction conviction;

    Card(const str& name, const str& nickname);
    void reset();

    bool isGuilty() const;
    bool isUnknown() const;
    bool isInnocent() const;

    bool ownerKnown(const size_t stageIndex) const;
    bool ownerUnknown(const size_t stageIndex) const;
    bool ownedBy(const Player* pPlayer, const size_t stageIndex) const;
    bool locationKnown(const size_t stageIndex) const;
    bool locationUnknown(const size_t stageIndex) const;

    bool operator<(const Card& card) const;
    bool operator==(const str& n) const;
};

#include "Globals.h"
