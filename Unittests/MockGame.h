#pragma once

#include "stdafx.h"

class MockGame : public Game
{
public:
    size_t refreshCalled = 0;
    std::vector<std::pair<str, str>> criticalCalled;

    void refresh()
    {
        ++refreshCalled;
    }

    void critical(const str& title, const str& desc)
    {
        criticalCalled.push_back(std::make_pair(title, desc));
    }

    std::wstring openCluedoTextFile(const str& issue)
    {
        return L"Cluedo.txt";
    }
};