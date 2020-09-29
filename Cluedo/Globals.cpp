#include "stdafx.h"
#include "Globals.h"

size_t g_numStages;
std::vector<Player> g_players;
std::vector<std::vector<Card>> g_cards;
std::vector<std::shared_ptr<const Turn>> g_pTurns;
