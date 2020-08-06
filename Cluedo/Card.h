#pragma once

enum class Conviction
{
    UNKNOWN,
    INNOCENT,
    GUILTY
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

    bool convictionKnown() const { return conviction != Conviction::UNKNOWN; }
    bool convictionUnknown() const { return conviction == Conviction::UNKNOWN; }
    bool ownedBy(const Player* pPlayer) const { return pOwner == pPlayer; }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
