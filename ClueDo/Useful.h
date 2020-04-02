#pragma once

#include <iostream>
#include <string>


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


bool readBool(const std::string& message)
{
    std::string read;
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


int readInt(const std::string& message, int min, int max)
{
    std::string read;
    while (true)
    {
        std::cout << message << std::endl;
        std::cin >> read;

        try
        {
            int answer = std::stoi(read);
            if (min <= answer || answer <= max)
            {
                return answer;
            }
        }
        catch (const std::invalid_argument&)
        {
            std::cout << read << " is not a number or not in range " << min << " - " << max;
        }
    }
}


std::string readString(const std::string& message)
{
    std::string read;
    std::cout << message << std::endl;
    std::cin >> read;
    return read;
}
