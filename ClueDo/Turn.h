#pragma once

enum class Action
{
    ASKED,
    MISSED,
    GUESSED
};

std::map<Action, std::string> actionStrings =
{
    {Action::ASKED,     " asked "},
    {Action::MISSED,    " missed a turn."},
    {Action::GUESSED,   " guessed "}
};


struct Turn
{
    Action action;
    bool cardShown;
    Player* pPlayer;
    std::vector<Card*> pCards;

    Turn(Player *pPlayer) :
        pPlayer(pPlayer)
    {
        pPlayer->pTurns.push_back(this);

        action = static_cast<Action>(readInt(pPlayer->name + "'s has:\n1.)\tAsked\n2.)\tMissed a turn\n3.)\tMade a guess", 1, 3) - 1);
        if (action != Action::MISSED)
        {
            while (pCards.size() != NUM_CATEGORIES)
            {
                Card* pCard = readCard();
                if (pCard != nullptr)
                {
                    pCards.push_back(pCard);
                }
            }
        }
    }

    bool missed() { return action == Action::MISSED; }

    std::string to_string()
    {
        std::string message = pPlayer->name + actionStrings[action];
        for (Card* pCard : pCards)
        {
            message += pCard->name + ", ";
        }

        return message;
    }
};

std::vector<Turn> g_turns;
