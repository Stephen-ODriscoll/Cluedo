#pragma once

#define MIN_PLAYERS 2
#define MAX_PLAYERS 6
#define NUM_CATEGORIES 3


struct contradiction : public std::exception
{
    using std::exception::exception;
};


enum class Mode
{
    NONE,
    SINGLE,
    GROUP
};

enum class Status
{
    OKAY,
    NEEDS_CARD_INFO,
    CONTRADICTION,
    EXCEPTION
};

const std::map<Status, str> statusStrings =
{
    { Status::OKAY,             "Status: <font color='green'>Okay</font>" },
    { Status::NEEDS_CARD_INFO,  "Status: <font color='yellow'>Needs Card Info</font>" },
    { Status::CONTRADICTION,    "Status: <font color='red'>Contradiction</font>" },
    { Status::EXCEPTION,        "Status: <font color='red'>Exception</font>" }
};


struct Player;
struct Card;
struct Category;
struct Turn;

extern size_t g_numStages;
extern str g_progressReport;
extern std::vector<Player> g_players;
extern std::vector<Category> g_categories;
extern std::vector<Player*> g_pPlayersOut;
extern std::vector<Player*> g_pPlayersLeft;
extern std::vector<std::vector<Card*>> g_wrongGuesses;
extern std::vector<std::shared_ptr<const Turn>> g_pTurns;


#include "Player.h"
#include "Card.h"
#include "Turn.h"
