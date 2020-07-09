#pragma once

struct Player;
struct Card
{
    const str name;
    const str nickname;
    Player* pOwner;

    Card(const str& name, const str& nickname) :
        name(name),
        nickname(nickname),
        pOwner(nullptr)
    { }

    bool ownerKnown() const { return pOwner; }
    bool ownerUnknown() const { return !pOwner; }
    bool ownedBy(const Player* pPlayer) const { return pOwner == pPlayer; }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
