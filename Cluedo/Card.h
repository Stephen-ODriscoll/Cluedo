#pragma once

enum class Status
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
    enum Status status;
    Player* pOwner;

    Card(const str& name, const str& nickname) :
        name(name),
        nickname(nickname),
        status(Status::UNKNOWN),
        pOwner(nullptr)
    { }

    bool ownerKnown() const { return pOwner; }
    bool ownerUnknown() const { return !pOwner; }
    bool ownedBy(const Player* pPlayer) const { return pOwner == pPlayer; }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
