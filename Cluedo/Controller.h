#pragma once

#include "Globals.h"

namespace fs = std::filesystem;

class Controller
{
    Mode m_mode;
    bool m_gameOver;

public:
    Controller(Mode mode, int numPlayers);
    void initialize(const fs::path& inputFile);
    
    void rename(const Player* pPlayer, const str& newName);
    void updatePresets(const Player* pPlayer, std::vector<StagePreset>& newPresets);

    void processTurn(std::shared_ptr<const Turn> pNewTurn, std::shared_ptr<const Turn> pOldTurn = nullptr);
    
private:
    void reAnalyseAll();
    void moveToBack(const Player* pPlayer);

    void analyseTurn(std::shared_ptr<const Turn> pTurn);
    void analyseAsked(std::shared_ptr<const Asked> pAsked);
    void analyseGuessed(std::shared_ptr<const Guessed> pGuessed);
};
