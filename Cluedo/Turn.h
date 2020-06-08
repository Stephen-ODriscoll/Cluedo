#pragma once

#include "stdafx.h"

enum class Action
{
    ASKED,
    MISSED,
    GUESSED
};

struct Turn
{
    Action action;
    bool outcome;
    Player* pDetective;
    Player* pWitness;
    std::vector<Card*> pCards;

    Turn(Player *pDetective, Player* pWitness) :
        pDetective(pDetective),
        pWitness(pWitness),
        outcome(false)
    {
        /*pDetective->pTurns.push_back(this);

        action = static_cast<Action>(readInt(pDetective->name + "'s has:\n1.)\tAsked\n2.)\tMissed a turn\n3.)\tMade a guess", 1, 3) - 1);
        if (action == Action::MISSED)
            return;

        while (pCards.size() != NUM_CATEGORIES)
        {
            Card* pCard = readCard(pCards.size() + 1);
            if (pCard != nullptr)
                pCards.push_back(pCard);
        }

        if (action == Action::ASKED)
            outcome = readBool(str("Did ") + pWitness->name + " have a card? (y/n)");
        else
            outcome = readBool(str("Was ") + pDetective->name + " right? (y/n)");*/
    }

    bool asked() const { return action == Action::ASKED; }
    bool missed() const { return action == Action::MISSED; }
    bool guessed() const { return action == Action::GUESSED; }

    str to_str() const
    {
        str message;
        switch (action)
        {
        case Action::ASKED:
            message = pWitness->name + str(outcome ? " has either " : " doesn't have ");
            break;
        case Action::GUESSED:
            message = pDetective->name + str(" guessed ") + str(outcome ? "correctly " : "incorrectly ");
            break;
        case Action::MISSED:
            message = pDetective->name + str(" missed a turn");
            break;
        }

        for (Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};
