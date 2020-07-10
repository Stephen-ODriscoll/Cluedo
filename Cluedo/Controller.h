#pragma once

#include "Macros.h"

#include "stdafx.h"
#include "Card.h"
#include "Player.h"
#include "Turn.h"
#include "Analysis.h"

enum class Mode
{
    NONE,
    SINGLE,
    GROUP
};

namespace fs = std::filesystem;

class Cluedo;
class Controller
{
    std::vector<std::vector<Card>> m_cards;
    std::vector<Player> m_players;
    std::vector<std::shared_ptr<const Turn>> m_pTurns;

    std::vector<Analysis> m_analysis;

    Mode m_mode;
    bool m_gameOver;
    int m_numPlayers;

    Cluedo* m_pGUI;

public:
    Controller(Cluedo* pGUI, fs::path inputFile = "ClueDo.txt");

    void startGame(Mode mode, int numPlayers);
    void analyseTurn(std::shared_ptr<const Turn> pTurn);
    void reAnalyseTurns(std::shared_ptr<const Turn> oldTurn, std::shared_ptr<const Turn> newTurn);

    bool rename(str oldName, str newName);

    std::vector<std::vector<Card>>& cards();
    const std::vector<Analysis>& analysis();
    const std::vector<Player>& players();
    const std::vector<std::shared_ptr<const Turn>>& turns();

private:
    void analysisSetup();

    void analyseMissed(std::shared_ptr<const Missed> pMissed);
    void analyseAsked(std::shared_ptr<const Asked> pAsked);
    void analyseGuessed(std::shared_ptr<const Guessed> pGuessed);
};

#include "Cluedo.h"