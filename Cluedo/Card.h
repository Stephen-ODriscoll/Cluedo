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
    std::vector<Player*> pOwners;

    Card(const str& name, const str& nickname) :
        name(name),
        nickname(nickname),
        conviction(Conviction::UNKNOWN),
        pOwners({ nullptr })
    { }

    void reset()
    {
        conviction = Conviction::UNKNOWN;
        pOwners = { nullptr };
    }

    bool isGuilty() const { return conviction == Conviction::GUILTY; }
    bool isUnknown() const { return conviction == Conviction::UNKNOWN; }
    bool isInnocent() const { return conviction == Conviction::INNOCENT; }

    bool ownerKnown(const size_t stageIndex) const { return pOwners[stageIndex]; }
    bool ownerUnknown(const size_t stageIndex) const { return !pOwners[stageIndex]; }
    bool ownedBy(const Player* pPlayer, const size_t stageIndex) const { return pOwners[stageIndex] == pPlayer; }
    bool locationKnown(const size_t stageIndex) const { return ownerKnown(stageIndex) || isGuilty(); }
    bool locationUnknown(const size_t stageIndex) const { return !locationKnown(stageIndex); }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
