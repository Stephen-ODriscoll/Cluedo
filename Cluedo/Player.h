#pragma once

#include "Macros.h"

struct Card;
struct StagePresets
{
    size_t numCards = 0;
    std::set<Card*> pCardsOwned;
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

struct Player
{
    str name;
    std::vector<StagePresets> stagePresets = { StagePresets() };

    bool out;
    std::vector<PlayerStage> stages;

    Player();
    void reset();

    bool processHas(Card* pCard, const size_t stageIndex);
    bool processDoesntHave(const std::vector<Card*>& pCards, const size_t stageIndex);
    bool processHasEither(const std::vector<Card*>& pCards, const size_t stageIndex);
    bool recheckCards(const size_t stageIndex);

    bool processGuessedWrong(const std::set<Card*>& guesserDoesntHave);

    str to_str(size_t stageIndex) const;

    bool operator!=(const Player& player) const;
    bool operator==(const str& n) const;
};

#include "Globals.h"
