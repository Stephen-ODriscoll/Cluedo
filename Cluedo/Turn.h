#pragma once

enum class Action
{
    MISSED,
    ASKED,
    GUESSED
};

struct Turn
{
    const size_t id;
    const Action action;
    const Player* pDetective;

    Turn::Turn(const Player* pDetective, const Action action, const size_t id) :
        pDetective(pDetective),
        action(action),
        id(id)
    { }

    str to_str() const;
    str witness() const;

    bool operator==(const Turn& turn) const { return id == turn.id; }
};


struct Missed : public Turn
{
    Missed(const Player* pDetective, const Action action, const size_t id) :
        Turn(pDetective, action, id)
    { }

    str to_str() const { return str(id) + str(".) ") + pDetective->name + str(" missed a turn"); }
};


struct Asked: public Turn
{
    const Player* pWitness;
    std::vector<Card*> pCards;
    const bool shown;
    const str cardShown;

    Asked(const Player* pDetective, const Action action, const size_t id,
            const Player* pWitness, std::vector<Card*> pCards, const bool shown, const str& cardShown = "") :
        Turn(pDetective, action, id),
        pWitness(pWitness),
        pCards(pCards),
        shown(shown),
        cardShown(cardShown)
    { assert(pCards.size() == NUM_CATEGORIES); }

    str to_str() const
    {
        str message = str(id) + str(".) ") + pWitness->name + str(shown ? " has either " : " doesn't have ");

        for (const Card* pCard : pCards)
            message += pCard->name + ", ";
        message.resize(message.size() - 2);

        return message;
    }
};


struct Guessed : public Turn
{
    std::vector<Card*> pCards;
    const bool correct;
    std::vector<int> redistribedCards;

    Guessed(const Player* pDetective, const Action action, const size_t id,
            std::vector<Card*> pCards, const bool correct) :
        Turn(pDetective, action, id),
        correct(correct),
        pCards(pCards)
    { assert(pCards.size() == NUM_CATEGORIES); }

    str to_str() const
    {
        str message = str(id) + str(".) ") + pDetective->name + str(" guessed ") + str(correct ? "correctly " : "incorrectly ");

        for (const Card* pCard : pCards)
            message += pCard->name + ", ";
        message.resize(message.size() - 2);

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
