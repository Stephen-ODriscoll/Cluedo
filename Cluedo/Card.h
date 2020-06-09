#pragma once

#include "stdafx.h"

struct Player;
struct Card
{
    const str name;
    const str nickname;
    const uint32_t category;
    Player* pOwner = nullptr;

    Card(const str& name, const str& nickname, const uint32_t category) :
        name(name),
        nickname(nickname),
        category(category)
    { }

    bool locationKnown() const { return pOwner == nullptr; }

    bool operator<(const Card& card) const { return name < card.name; }
    bool operator==(const str& n) const { return name == n; }
};
