#pragma once

#include "stdafx.h"

enum class Action
{
    MISSED,
    ASKED,
    GUESSED
};


struct Turn
{
    Action action;
    Player* pDetective;

    Turn(Player* pDetective, Action action) :
        pDetective(pDetective),
        action(action)
    { }
};


struct Missed : public Turn
{
    str to_str() const
    {
        return pDetective->name + str(" missed a turn");
    }
};


struct Asked: public Turn
{
    Player* pWitness;
    bool shown;
    std::vector<Card*> pCards;
    str cardShown;

    Asked(Player* pDetective, Action action, Player* pWitness, bool shown, std::vector<Card*> pCards, const str& cardShown = "") :
        Turn(pDetective, action),
        pWitness(pWitness),
        shown(shown),
        pCards(pCards),
        cardShown(cardShown)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    str to_str() const
    {
        str message = pWitness->name + str(shown ? " has either " : " doesn't have ");

        for (Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};


struct Guessed : public Turn
{
    bool correct;
    std::vector<Card*> pCards;

    Guessed(Player* pDetective, Action action, bool correct, std::vector<Card*> pCards) :
        Turn(pDetective, action),
        correct(correct),
        pCards(pCards)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    str to_str() const
    {
        str message = pDetective->name + str(" guessed ") + str(correct ? "correctly " : "incorrectly ");

        for (Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};

