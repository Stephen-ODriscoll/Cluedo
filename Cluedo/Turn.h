#pragma once

enum class Action
{
    MISSED,
    ASKED,
    GUESSED
};


struct Turn
{
    const Action action;
    const Player* pDetective;

    Turn(const Player* pDetective, const Action action) :
        pDetective(pDetective),
        action(action)
    { }
};


struct Missed : public Turn
{
    Missed(const Player* pDetective, const Action action) :
        Turn(pDetective, action)
    { }

    str to_str() const
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
        str message = pWitness->name + str(shown ? " has either " : " doesn't have ");

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
        str message = pDetective->name + str(" guessed ") + str(correct ? "correctly " : "incorrectly ");

        for (const Card* pCard : pCards)
            message += pCard->name + ", ";

        return message;
    }
};

