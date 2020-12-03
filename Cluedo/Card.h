#pragma once

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
    CardStage(std::vector<Player*>& pPlayers);
};

struct Card
{
    const str name, nickname;
    const size_t categoryIndex;

    std::vector<CardStage> stages;
    enum class Conviction conviction;

    Card(const str& name, const str& nickname, const size_t categoryIndex);
    void reset();

    bool processBelongsTo(Player* pPlayer, const size_t stageIndex);
    bool processDoesntBelongTo(Player* pPlayer, const size_t stageIndex);
    bool processGuilty();
    bool recheck();

    void processGuessedWrong(Player* pPlayer);

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

struct Category
{
    bool guiltyKnown = false;
    std::vector<Card> cards;

    void reset();
};

#include "Globals.h"
