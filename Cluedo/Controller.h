#pragma once

#include "Macros.h"

#include "Cards.h"
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
    std::vector<Turn*> m_pTurns;

    std::set<Card*> m_pPossibleCards;
    std::vector<Analysis> m_analysis;

    Mode m_mode;
    bool m_gameOver;
    int m_numPlayers;

    Cluedo* m_pGUI;

public:
    Controller(Cluedo* pGUI, fs::path inputFile = "ClueDo.txt");
    ~Controller();

    void startGame(Mode mode, int numPlayers);
    void analyseMissed(Missed* pMissed);
    void analyseAsked(Asked* pAsked);
    void analyseGuessed(Guessed* pGuessed);

    bool rename(str oldName, str newName);

    const std::vector<std::vector<Card>>& cards();
    const std::vector<Player>& players();
    const std::vector<Turn*>& turns();

private:
    void analysisSetup();
};

#include "Cluedo.h"