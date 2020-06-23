#pragma once

struct Player;
struct Card
{
    const str name;
    const str nickname;
    Player* pOwner = nullptr;

    Card(const str& name, const str& nickname) :
        name(name),
        nickname(nickname)
    { }

    bool locationKnown() const { return pOwner == nullptr; }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
