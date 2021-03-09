#pragma once

struct Card;
struct StagePreset
{
    size_t numCards = 0;
    std::set<Card*> pCardsOwned;

    StagePreset();
    StagePreset(const std::set<Card*> pCardsOwned);
    StagePreset(const size_t numCards, const std::set<Card*> pCardsOwned);

    bool isNumCardsKnown() const;
    bool operator==(const StagePreset& stagePreset) const;
};

// A stage refers to the time between guesses.
// E.g. game starts and we enter stage 1, a player
// guesses wrong thus the cards get passed around
// and we enter stage 2.
struct PlayerStage
{
    std::set<Card*> has;
    std::set<Card*> doesntHave;
    std::vector<std::vector<Card*>> hasEither;

    PlayerStage();
    PlayerStage(std::set<Card*> has, std::set<Card*> doesntHave, std::vector<std::vector<Card*>> hasEither);
};

#include "Perspective.h"

struct Player : public Perspective
{
    str name;
    std::vector<StagePreset> presets = { StagePreset() };

    std::vector<PlayerStage> stages;

    Player();
    bool reset();

    bool processHas(Card* pCard, const size_t stageIndex);
    bool processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex);
    bool processHasEither(const std::vector<Card*>& pCards, const size_t stageIndex);
    bool processGuessedWrong(Player* pPlayer, int cardsReceived = -1);
    bool recheck();

    bool allCardsKnown(size_t stageIndex) const;

    str to_str(size_t stageIndex) const;

    bool operator!=(const Player& player) const;
    bool operator==(const str& n) const;
};

#include "Globals.h"
