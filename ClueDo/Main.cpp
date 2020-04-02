#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "Useful.h"
#include "Global.h"

namespace fs = std::filesystem;


void loadCards(const fs::path& inputFile)
{
    std::ifstream load(inputFile);

    if (!load.is_open())
    {
        throw std::invalid_argument(std::string("Failed to load file ") + inputFile.string());
    }

    std::string line;
    std::cout << "Category 1: ";
    g_cards.push_back(std::vector<Card>());
    for (uint32_t category = 0U; std::getline(load, line);)
    {
        if (line.empty())
        {
            if (!g_cards[category].empty())
            {
                g_cards.push_back(std::vector<Card>());
                ++category;

                std::cout << "\nCategory " << (category + 1) << ": ";
            }

            continue;
        }

        g_cards[category].push_back(line);
        std::cout << line << ", ";
    }

    load.close();
    std::cout << std::endl;

    if (g_cards.size() != NUM_CATEGORIES)
    {
        throw std::exception((std::string("There should be ") + std::to_string(NUM_CATEGORIES) + std::string(" categories.")).c_str());
    }
}


void startingInfo()
{
    g_numPlayers = readInt("How many players?", MIN_PLAYERS, MAX_PLAYERS);
    g_cardsEvenlyDistributed = (g_cards.size() % g_numPlayers) == 0 && readBool("Have the cards been distributed evenly? (y/n)");
    g_players = std::vector<Player>(g_numPlayers);
}


void analyse(Player& perspective)
{

}


void playGame()
{
    while (true)
    {
        for (Player& player : g_players)
        {
            switch (readInt(std::string("Mode Selected: ") + modeStrings[g_mode]\
                + std::string("Menu:\n")\
                + std::string("1.)\t" + player.name + "'s turn")
                , 1, 1))
            {
            case 1:
                g_turns.push_back(Turn(&player));
                break;
            }

            if (g_gameOver)
            {
                return;
            }
        }
    }
}


int main(int argc, char* argv[])
{
    auto exitCode = EXIT_SUCCESS;

    try
    {
        fs::path inputFile("ClueDo.txt");
        for (int i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--hide"))
            {
                (g_mode == Mode::NONE) ?
                    g_mode = Mode::HIDE_ALL :
                    throw std::invalid_argument("Mode already set, please only set mode once");
            }
            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--events"))
            {
                (g_mode == Mode::NONE) ?
                    g_mode = Mode::EVENTS_ONLY :
                    throw std::invalid_argument("Mode already set, please only set mode once");
            }
            else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--show"))
            {
                (g_mode == Mode::NONE) ?
                    g_mode = Mode::SHOW_ALL :
                    throw std::invalid_argument("Mode already set, please only set mode once");
            }
            else if (inputFile.empty())
            {
                (fs::exists(inputFile)) ?
                    inputFile = argv[i] :
                    throw std::invalid_argument(std::string("Argument isn't a valid path - ") + argv[i]);
            }
            else
            {
                throw std::invalid_argument(std::string("Unrecognised argument ") + argv[i]);
            }
        }

        switch (g_mode)
        {
        case Mode::NONE:
            throw std::invalid_argument("No mode chosen");
        case Mode::HIDE_ALL:
            std::cout << "Only obvious info will be shown during game." << std::endl;
            break;
        case Mode::EVENTS_ONLY:
            std::cout << "Only events will be shown during the game." << std::endl;
            break;
        case Mode::SHOW_ALL:
            std::cout << "All information will be shown." << std::endl;
            break;
        }

        // Calling gameplay functions
        loadCards(inputFile);
        startingInfo();
        playGame();
    }
    catch (const std::invalid_argument& ex)
    {
        std::cout   << "Invalid argument: " << ex.what() << "\n"
                    << "Arguments are:\n"
                    << "        Mode:\n"
                    << "            -h, --hide      Don't show any info until the game is over\n"
                    << "            -e, --events    Only display events; What was asked by who\n"
                    << "            -s, --show      Display info during the game, including who has what cards\n"
                    << "\n"
                    << "        path/to/file    Path to details file containing card names"
                    << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
        exitCode = EXIT_FAILURE;
    }

    return exitCode;
}