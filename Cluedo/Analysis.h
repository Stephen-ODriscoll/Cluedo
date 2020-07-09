#pragma once

struct contradiction : public std::exception
{
    using std::exception::exception;
};

struct Analysis
{
    Player* pPlayer;
    std::set<Card*> has;
    std::set<Card*> doesntHave;
    std::vector<std::vector<Card*>> hasEither;

    Analysis(Player* pPlayer):
        pPlayer(pPlayer) { }

    bool processHas(Card* pCard)
    {
        if (pCard->ownerKnown())
        {
            // We already know that this person owns this card, no new info
            if (pCard->ownedBy(pPlayer))
                return false;

            throw contradiction((
                str("Deduced that ") + pCard->name + str(" is owned by ") + pPlayer->name +
                str(" but this card is already owned by ") + pCard->pOwner->name).c_str());
        }

        // We've found a card and have new info to go on
        pCard->pOwner = pPlayer;
        has.insert(pCard);
        return true;
    }

    bool processDoesntHave(const std::vector<Card*> pCards)
    {
        for (Card* pCard : pCards)
        {
            if (pCard->ownedBy(pPlayer))
                throw contradiction((str("Previous info says ") + pPlayer->name + str(" has ") + pCard->name).c_str());

            if (pCard->ownerUnknown())
                doesntHave.insert(pCard);
        }

        return recheckCards();
    }

    bool processHasEither(const std::vector<Card*> pCards)
    {
        std::vector<Card*> checkedCards;
        for (Card* pCard : pCards)
        {
            // This card could've been shown if this player hasn't said no to it and
            // either we don't know who owns it or they own it
            if (doesntHave.find(pCard) == doesntHave.end() &&
                (pCard->ownerUnknown() || pCard->ownedBy(pPlayer)))
                checkedCards.emplace_back(pCard);
        }

        switch (checkedCards.size())
        {
        case 0:
            throw contradiction((str("Player ") + pPlayer->name + str(" must have one of these cards")).c_str());

        case 1:
            return processHas(checkedCards[0]);

        default:
            hasEither.push_back(checkedCards);      // We don't know for sure which card was shown (yet)
        }

        return false;
    }

    bool recheckCards()
    {
        for (auto it = doesntHave.begin(); it != doesntHave.end();)
        {
            // If card location is known remove it from doesn't have.
            // We already know they don't have it because someone else does.
            if ((*it)->ownerKnown())
                it = doesntHave.erase(it);
            else
                ++it;
        }

        // Recheck past cards with this new info
        bool cardFound = false;
        for (auto it1 = hasEither.begin(); it1 != hasEither.end();)
        {
            for (auto it2 = it1->begin(); it2 != it1->end();)
            {
                // This card could've been shown if this player hasn't said no to it and
                // either we don't know who owns it or they own it
                if (doesntHave.find(*it2) == doesntHave.end() &&
                    ((*it2)->ownerUnknown() || (*it2)->ownedBy(pPlayer)))
                    ++it2;
                else
                    it2 = it1->erase(it2);
            }

            switch (it1->size())
            {
            case 0:
                throw contradiction((str("Player ") + pPlayer->name + str(" must have one of these cards")).c_str());

            case 1:
                cardFound = processHas((*it1)[0]);
                it1 = hasEither.erase(it1);
                break;

            default:
                ++it1;
            }
        }

        return cardFound;
    }

    str to_str() const
    {
        str info = pPlayer->name + str("\n\thas: ");
        for (const Card* pCard : has)
            info += pCard->nickname + str(", ");
        if (!has.empty())
            info.resize(info.size() - 2);

        info += str("\n\thas either: ");
        for (const std::vector<Card*>& pCards : hasEither)
        {
            for (const Card* pCard : pCards)
                info += pCard->nickname + str("/");

            info.resize(info.size() - 1);
            info += ", ";
        }
        if (!hasEither.empty())
            info.resize(info.size() - 2);

        info += "\n\tdoesn't have: ";
        for (const Card* pCard : doesntHave)
            info += pCard->nickname + str(", ");
        if (!doesntHave.empty())
            info.resize(info.size() - 2);

        return info + "\n";
    }

    bool operator==(const Analysis& a) { return pPlayer == a.pPlayer; }
    bool operator==(const Player* p) { return pPlayer == p; }
};
