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
    PlayerStage(const std::set<Card*>& has, const std::set<Card*>& doesntHave, const std::vector<std::vector<Card*>>& hasEither);
};

struct Player
{
    str name;
    std::vector<StagePreset> presets = { StagePreset() };

    std::vector<PlayerStage> stages;

    Player();
    void reset();

    void processHas(Card* pCard, const size_t stageIndex);
    void processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex);
    void processHasEither(const std::vector<Card*>& pCards, const size_t stageIndex);
    void processGuessedWrong(Player* pGuesser, const int cardsReceived = -1);
    void recheckHasEither();

    bool isIn(const size_t stageIndex);
    bool allCardsKnown(const size_t stageIndex) const;

    str to_str(const size_t stageIndex) const;

    bool operator!=(const Player& player) const;
    bool operator==(const str& n) const;
};

#include "Globals.h"
