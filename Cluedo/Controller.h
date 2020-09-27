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

    std::vector<Analysis> m_analyses;

    Mode m_mode;
    bool m_gameOver;
    size_t m_numStages;

    Cluedo* m_pGUI;

public:
    Controller(Cluedo* pGUI, fs::path inputFile = "ClueDo.txt");

    void startGame(Mode mode, int numPlayers);
    void processNewTurn(std::shared_ptr<const Turn> pTurn);

    bool rename(const Player* pPlayer, const str& newName);
    void updateHasCards(const Player* pPlayer, const std::vector<str>& cardNames, const size_t stageIndex);
    void replaceTurn(std::shared_ptr<const Turn> oldTurn, std::shared_ptr<const Turn> newTurn);

    size_t numStages();
    size_t playersLeft();
    std::vector<std::vector<Card>>& cards();
    const std::vector<Player>& players();
    const std::vector<Analysis>& analyses();
    const std::vector<std::shared_ptr<const Turn>>& turns();

private:
    void analysesSetup();
    void reAnalyseTurns();
    void processHasAtStage(const size_t stageIndex);

    void analyseTurn(std::shared_ptr<const Turn> pTurn);
    void analyseAsked(std::shared_ptr<const Asked> pAsked);
    void analyseGuessed(std::shared_ptr<const Guessed> pGuessed);
    void continueDeducing();
    bool exteriorChecks();
};

#include "Cluedo.h"