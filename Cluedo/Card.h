#pragma once

enum class Conviction : uint8_t
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

    void processGuilty();
    void processInnocent();

    void processBelongsTo(Player* pPlayer, const size_t stageIndex);
    void processDoesntBelongTo(Player* pPlayer, const size_t stageIndex);
    void processGuessedWrong(Player* pGuesser);
    void recheckLocation();

    bool ownerKnown(const size_t stageIndex) const;
    bool locationKnown(const size_t stageIndex) const;
    bool couldBelongTo(Player* pPlayer, const size_t stageIndex) const;
    bool ownedBy(const Player* pPlayer, const size_t stageIndex) const;

    bool operator<(const Card& card) const;
    bool operator==(const str& n) const;
};

struct Category
{
    std::vector<Card> cards;
    std::set<Card*> pPossibleGuilty;

    Category(const std::vector<Card> &cards);

    void reset();
    void recheckGuilty();
};

#include "Globals.h"
