#pragma once

#include "pch.h"

#define BLANK "."
#define MIN_PLAYERS 2
#define MAX_PLAYERS 6
#define NUM_CATEGORIES 3


enum class Mode
{
    NONE,
    HIDE_ALL,
    EVENTS_ONLY,
    SHOW_ALL
};

std::map<Mode, str> modeStrings =
{
    {Mode::NONE,        "None. Something's gone wrong"},
    {Mode::HIDE_ALL,    "Hide All"  },
    {Mode::EVENTS_ONLY, "Events Only"},
    {Mode::SHOW_ALL,    "Show All"}
};


int g_numPlayers = 0;
bool g_gameOver = false;
Mode g_mode = Mode::NONE;
bool g_cardsEvenlyDistributed = false;

#include "Cards.h"
#include "Player.h"
#include "Turn.h"
#include "Analysis.h"
