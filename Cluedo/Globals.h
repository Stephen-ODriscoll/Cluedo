#pragma once

struct contradiction : public std::exception
{
    using std::exception::exception;
};

struct Player;
struct Card;
struct Turn;

extern size_t g_numStages;
extern std::vector<Player> g_players;
extern std::vector<std::vector<Card>> g_cards;
extern std::vector<std::shared_ptr<const Turn>> g_pTurns;

#include "Player.h"
#include "Card.h"
#include "Turn.h"