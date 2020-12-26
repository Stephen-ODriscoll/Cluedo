#pragma once

#include "Globals.h"

enum class Action : uint8_t
{
    MISSED,
    ASKED,
    GUESSED
};

struct Turn
{
    const Action action;
    const Player* pDetective;

    Turn::Turn(const Player* pDetective, const Action action) :
        pDetective(pDetective),
        action(action)
    { }

    virtual bool shouldRedistribute() const { return false; }
    virtual str witness() const { return ""; }
    virtual str to_str() const = 0;
};


struct Missed : public Turn
{
    Missed(const Player* pDetective) :
        Turn(pDetective, Action::MISSED)
    { }

    str to_str() const override
    {
        return pDetective->name + str(" missed a turn");
    }
};


struct Asked: public Turn
{
    const Player* pWitness;
    std::vector<Card*> pCards;
    const bool shown;
    const str cardShown;

    Asked(const Player* pDetective, const Player* pWitness, std::vector<Card*> pCards, const bool shown, const str& cardShown = "") :
        Turn(pDetective, Action::ASKED),
        pWitness(pWitness),
        pCards(pCards),
        shown(shown),
        cardShown(cardShown)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    str witness() const override
    {
        return pWitness->name;
    }

    str to_str() const override
    {
        return pWitness->name + str(shown ? " has either " : " doesn't have ")
            + str(pCards.begin(), pCards.end(), [](Card* pCard) { return pCard->name; });
    }
};


struct Guessed : public Turn
{
    std::vector<Card*> pCards;
    const bool correct;
    std::vector<int> redistribedCards;

    Guessed(const Player* pDetective, std::vector<Card*> pCards, const bool correct) :
        Turn(pDetective, Action::GUESSED),
        correct(correct),
        pCards(pCards)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    bool shouldRedistribute() const override
    {
        return !correct;
    }

    str to_str() const override
    {
        return pDetective->name + str(" guessed ") + str(correct ? "correctly " : "incorrectly ")
            + str(pCards.begin(), pCards.end(), [](Card* pCard) { return pCard->name; });
    }
};
