#pragma once

enum class Conviction
{
    UNKNOWN,
    INNOCENT,
    GUILTY
};

const std::map<Conviction, str> convictionStrings =
{
    { Conviction::UNKNOWN,  "" },
    { Conviction::INNOCENT, "Innocent" },
    { Conviction::GUILTY,   "Guilty" }
};

struct Player;
struct Card
{
    const str name;
    const str nickname;
    enum class Conviction conviction;
    Player* pOwner;

    Card(const str& name, const str& nickname) :
        name(name),
        nickname(nickname),
        conviction(Conviction::UNKNOWN),
        pOwner(nullptr)
    { }

    void reset()
    {
        conviction = Conviction::UNKNOWN;
        pOwner = nullptr;
    }

    bool isGuilty() const { return conviction == Conviction::GUILTY; }
    bool isUnknown() const { return conviction == Conviction::UNKNOWN; }
    bool isInnocent() const { return conviction == Conviction::INNOCENT; }

    bool ownerKnown() const { return pOwner; }
    bool ownerUnknown() const { return !pOwner; }
    bool ownedBy(const Player* pPlayer) const { return pOwner == pPlayer; }
    bool locationKnown() const { return ownerKnown() || isGuilty(); }
    bool locationUnknown() const { return !locationKnown(); }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
