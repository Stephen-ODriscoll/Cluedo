#pragma once

#include "stdafx.h"
#include "Globals.h"

enum class Mode
{
    NONE,
    SINGLE,
    GROUP
};

namespace fs = std::filesystem;

class Game;
class Controller
{
    Mode m_mode;
    bool m_gameOver;

    Game* m_pGame;

public:
    Controller(Game* pGame, Mode mode, int numPlayers);

    void processTurn(std::shared_ptr<const Turn> pTurn);
    bool rename(const Player* pPlayer, const str& newName);
    void updatePresets(const Player* pPlayer, std::vector<StagePreset>& newPresets);

private:
    void analysesSetup();
    void reAnalyseTurns();
    void moveToBack(const Player* pPlayer);

    void analyseTurn(std::shared_ptr<const Turn> pTurn);
    void analyseAsked(std::shared_ptr<const Asked> pAsked);
    void analyseGuessed(std::shared_ptr<const Guessed> pGuessed);
    void continueDeducing();
    bool exteriorChecks();
};

#include "Game.h"