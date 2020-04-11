#pragma once

#include "pch.h"

void clear()
{
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}


bool readBool(const str& message)
{
    str read;
    while (true)
    {
        std::cout << message << std::endl;
        std::cin >> read;

        switch (read[0])
        {
        case 'y':
            return true;
        case 'n':
            return false;
        }
    }
}


int readInt(const str& message, int min, int max)
{
    str read;
    while (true)
    {
        std::cout << message << std::endl;
        std::cin >> read;

        try
        {
            int answer = read.toi();
            if (min <= answer || answer <= max)
                return answer;
        }
        catch (const std::invalid_argument&)
        {
            std::cout << read << " is not a number or not in range " << min << " - " << max;
        }
    }
}


str readStr(const str& message)
{
    str read;
    std::cout << message << std::endl;
    std::cin >> read;
    return read;
}


bool caseInsensitiveFind(str parent, str child)
{
    std::transform(parent.begin(), parent.end(), parent.begin(),
        [](char c) -> char { return std::tolower(c); });
    std::transform(child.begin(), child.end(), child.begin(),
        [](char c) -> char { return std::tolower(c); });

    return (parent.find(child) != str::npos);
}
