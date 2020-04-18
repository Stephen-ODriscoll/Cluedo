#pragma once

#include "stdafx.h"

struct Analysis
{
    Player* pPlayer;
    std::set<Card*> has;
    std::set<Card*> doesntHave;
    std::vector<std::vector<Card*>> hasEither;

    Analysis(Player* pPlayer):
        pPlayer(pPlayer) { }

    bool processHasEither(std::vector<Card*> pCards, std::set<Card*> possibleCards)
    {
        std::vector<Card*> checkedCards;
        for (Card* pCard : pCards)
        {
            // This card could've been shown if this player hasn't said no to it and
            // either it's not suspected or this player is known to have it
            if (doesntHave.find(pCard) == doesntHave.end() &&
                (possibleCards.find(pCard) != possibleCards.end() || has.find(pCard) != has.end()))
                checkedCards.emplace_back(pCard);
        }

        if (checkedCards.size() == 1)
        {
            has.insert(checkedCards[0]);            // We've found a card
            checkedCards[0]->pOwner = pPlayer;
            possibleCards.erase(checkedCards[0]);   // Obviously this card is no longer suspected

            return true;
        }
        else if (checkedCards.size() < 1)
            hasEither.push_back(checkedCards);      // We don't know for sure which card was shown (yet)

        return false;
    }

    bool processDoesntHave(std::vector<Card*> pCards, std::set<Card*> possibleCards)
    {
        bool cardFound = false;
        for (Card* pCard : pCards)
        {
            if (possibleCards.find(pCard) != possibleCards.end())
            {
                doesntHave.insert(pCard);
                for (std::vector<Card*>& pCards : hasEither)
                {
                    for (auto it = pCards.begin(); it < pCards.end(); ++it)
                    {
                        if (*it == pCard)
                        {
                            pCards.erase(it);
                            break;
                        }
                    }

                    if (pCards.size() == 1)
                    {
                        has.insert(pCards[0]);            // We've found a card
                        pCards[0]->pOwner = pPlayer;
                        possibleCards.erase(pCards[0]);   // Obviously this card is no longer suspected

                        cardFound = true;
                    }
                }
            }
        }

        return cardFound;
    }

    bool processPossibleCards(std::set<Card*> possibleCards)
    {
        for (auto it = doesntHave.begin(); it != doesntHave.end();)
        {
            if (possibleCards.find(*it) != possibleCards.end())
                doesntHave.erase(it);
            else
                ++it;
        }

        // Recheck past turns with this new info
        bool cardFound = false;
        for (auto it1 = hasEither.begin(); it1 < hasEither.end();)
        {
            for (auto it2 = it1->begin(); it2 < it1->end();)
            {
                // This card could've been shown if this player hasn't said no to it and
                // either it's not suspected or this player is known to have it
                if (doesntHave.find(*it2) == doesntHave.end() &&
                    (possibleCards.find(*it2) != possibleCards.end() || has.find(*it2) != has.end()))
                    it1->erase(it2);
                else
                    ++it2;
            }

            if (it1->size() == 1)
            {
                has.insert((*it1)[0]);              // We've found a card
                (*it1)[0]->pOwner = pPlayer;
                possibleCards.erase((*it1)[0]);     // Obviously this card is no longer suspected
                hasEither.erase(it1);               // Removing from a large vector (RIP performance) :'(

                cardFound = true;
            }
            else
                ++it1;
        }

        return cardFound;
    }

    str to_str()
    {
        str info = pPlayer->name + str("\n\thas: ");
        for (Card* pCard : has)
            info += pCard->name + str(", ");
        if (!has.empty())
            info.resize(info.size() - 2);

        info += str("\n\thas either: ");
        for (std::vector<Card*>& pCards : hasEither)
        {
            for (Card* pCard : pCards)
                info += pCard->name + str("/");

            info.resize(info.size() - 1);
            info += ", ";
        }
        if (!hasEither.empty())
            info.resize(info.size() - 2);

        info += "\n\tdoesn't have: ";
        for (Card* pCard : doesntHave)
            info += pCard->name + str(", ");
        if (!doesntHave.empty())
            info.resize(info.size() - 2);

        return info;
    }

    bool operator==(const Analysis& a)
    {
        return pPlayer == a.pPlayer;
    }
    bool operator==(const Turn& t)
    {
        return pPlayer == t.pWitness;
    }
};

std::set<Card*> g_possibleCards;
std::vector<Analysis> g_analysis;


void analyseAsk(Turn& turn)
{
    auto it = std::find(g_analysis.begin(), g_analysis.end(), turn);
    if (it != g_analysis.end())
    {
        bool cardDeduced;
        if (turn.outcome)
            cardDeduced = it->processHasEither(turn.pCards, g_possibleCards);
        else
            cardDeduced = it->processDoesntHave(turn.pCards, g_possibleCards);
        
        // This could loop a few times. One deduction could lead to another and so on
        while (cardDeduced)
        {
            cardDeduced = false;
            for (auto aIt = g_analysis.begin(); aIt < g_analysis.end(); ++aIt)
                cardDeduced |= aIt->processPossibleCards(g_possibleCards);
        }
    }
}


void analyseTurn(Turn& turn)
{
    switch (turn.action)
    {
    case Action::ASKED:
        analyseAsk(turn);
        break;
    case Action::GUESSED:
        break;
    }
}


void analysisSetup()
{
    for (Player& player : g_players)
        g_analysis.push_back(&player);

    for (std::vector<Card>& category : g_cards)
    {
        for (Card& card : category)
        {
            if (card.pOwner == nullptr)         // Unowned, we suspect it
                g_possibleCards.insert(&card);
            else
            {
                auto it = std::find(g_analysis.begin(), g_analysis.end(), card.pOwner);
                if (it != g_analysis.end())
                    it->has.insert(&card);
            }
        }
    }
}


void analyseGame(Player& perspective)
{
    /*std::vector<Card*> possibleCards;
    for (std::vector<Card>& category : g_cards)
        for (Card& card : category)
            possibleCards.push_back(&card);

    std::vector<Analysis> analyse;
    for (Player& player : g_players)
        if (perspective != player)
            analyse.push_back(&player);

    for (Analysis& analysis : analyse)
    {

    }*/
}
