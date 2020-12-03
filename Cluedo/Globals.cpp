#include "stdafx.h"
#include "Globals.h"

size_t g_numStages;
str g_progressReport;
std::vector<Player> g_players;
std::vector<Category> g_categories;
std::vector<Player*> g_pPlayersOut;
std::vector<Player*> g_pPlayersLeft;
std::vector<std::vector<Card*>> g_wrongGuesses;
std::vector<std::shared_ptr<const Turn>> g_pTurns;
