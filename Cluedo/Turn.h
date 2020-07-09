#pragma once

#include "Macros.h"
#include "Card.h"
#include "Player.h"

enum class Action
{
    MISSED,
    ASKED,
    GUESSED
};

struct Turn
{
    const int id;
    const Action action;
    const Player* pDetective;

    Turn::Turn(const Player* pDetective, const Action action, const int id = 0) :
        pDetective(pDetective),
        action(action),
        id(id ? id : nextId())
    { }

    str to_str() const;
    str witness() const;

private:
    int nextId()
    {
        static int nextId = 0;
        return ++nextId;
    }
};


struct Missed : public Turn
{
    Missed(const Player* pDetective, const Action action) :
        Turn(pDetective, action)
    { }

    str to_str() const
    {
        return str(id) + str(".) ") + pDetective->name + str(" missed a turn");
    }
};


struct Asked: public Turn
{
    const Player* pWitness;
    std::vector<Card*> pCards;
    const bool shown;
    const str cardShown;

    Asked(const Player* pDetective, const Action action,
            const Player* pWitness, std::vector<Card*> pCards, const bool shown, const str& cardShown = "") :
        Turn(pDetective, action),
        pWitness(pWitness),
        pCards(pCards),
        shown(shown),
        cardShown(cardShown)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    str to_str() const
    {
        str message = str(id) + str(".) ") + pWitness->name + str(shown ? " has either " : " doesn't have ");

        for (const Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};


struct Guessed : public Turn
{
    std::vector<Card*> pCards;
    const bool correct;

    Guessed(const Player* pDetective, const Action action,
            std::vector<Card*> pCards, const bool correct) :
        Turn(pDetective, action),
        correct(correct),
        pCards(pCards)
    {
        assert(pCards.size() == NUM_CATEGORIES);
    }

    str to_str() const
    {
        str message = str(id) + str(".) ") + pDetective->name + str(" guessed ") + str(correct ? "correctly " : "incorrectly ");

        for (const Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};


inline str Turn::to_str() const
{
    switch (action)
    {
    case Action::MISSED:
        return static_cast<const Missed*>(this)->to_str();

    case Action::ASKED:
        return static_cast<const Asked*>(this)->to_str();

    case Action::GUESSED:
        return static_cast<const Guessed*>(this)->to_str();
    }

    return "";
}

inline str Turn::witness() const
{
    if (action == Action::ASKED)
        return static_cast<const Asked*>(this)->pWitness->name;

    return "";
}
