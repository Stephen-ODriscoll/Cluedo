#include "stdafx.h"
#include "Cluedo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Cluedo w;
    w.show();
    return a.exec();
}

//#include "Useful.h"
//#include "Global.h"
//
//namespace fs = std::filesystem;
//
//
//void loadCards(const fs::path& inputFile)
//{
//    if (!fs::exists(inputFile))
//        throw std::invalid_argument(str("Argument isn't a valid path - ") + inputFile.string());
//
//    std::ifstream load(inputFile);
//
//    if (!load.is_open())
//        throw std::invalid_argument(str("Failed to load file ") + inputFile.string());
//
//    str line;
//    std::cout << "Category 1: ";
//    for (uint32_t category = 0U; std::getline(load, line);)
//    {
//        if (NUM_CATEGORIES <= category)
//            throw std::exception((str("There should be ") + str(NUM_CATEGORIES) + str(" categories.")).c_str());
//
//        if (line.empty())
//        {
//            if (!g_cards[category].empty())
//            {
//                ++category;
//                std::cout << "\nCategory " << (category + 1) << ": ";
//            }
//
//            continue;
//        }
//
//        ++g_numCards;
//        g_cards[category].push_back(Card(line, category));
//        std::cout << line << ", ";
//    }
//
//    load.close();
//    std::cout << std::endl;
//}
//
//
//void startingInfo()
//{
//    g_numPlayers = readInt("How many players?", MIN_PLAYERS, MAX_PLAYERS);
//    g_cardsEvenlyDistributed = (g_numCards % g_numPlayers) == 0 && readBool("Have the cards been distributed evenly? (y/n)");
//    g_players = std::vector<Player>(g_numPlayers);
//}
//
//
//void playGame()
//{
//    //analysisSetup();
//    str turnsString("Turns:\n");
//
//    for (auto itCurrent = g_players.begin(), itNext = itCurrent + 1; true;)
//    {
//        std::cout << "Mode Selected: " << modeStrings[g_mode] << "\n";
//        std::cout << turnsString << std::endl;
//        switch (readInt(str("Menu:\n")\
//            + str("1.)\t" + itCurrent->name + "'s turn\n")
//            + str("2.)\tMore info")
//            , 1, 1))
//        {
//        case 1:
//            g_turns.push_back(Turn(&*itCurrent, &*itNext));
//            turnsString += str("\t") + g_turns.back().to_str() + "\n";
//            //analyseTurn(g_turns.back());
//            break;
//        case 2:
//            for (Analysis& analysis : g_analysis)
//                std::cout << analysis.to_str() << "\n";
//            break;
//        }
//
//        if (g_gameOver)
//            return;
//
//        itCurrent = itNext;
//        if (++itNext == g_players.end())
//            itNext = g_players.begin();
//    }
//}
//
//
//int main(int argc, char* argv[])
//{
//    auto exitCode = EXIT_SUCCESS;
//
//    try
//    {
//        fs::path inputFile;
//        for (int i = 1; i < argc; ++i)
//        {
//            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--hide"))
//            {
//                (g_mode == Mode::NONE) ?
//                    g_mode = Mode::HIDE_ALL :
//                    throw std::invalid_argument("Mode already set, please only set mode once");
//            }
//            else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--events"))
//            {
//                (g_mode == Mode::NONE) ?
//                    g_mode = Mode::EVENTS_ONLY :
//                    throw std::invalid_argument("Mode already set, please only set mode once");
//            }
//            else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--show"))
//            {
//                (g_mode == Mode::NONE) ?
//                    g_mode = Mode::SHOW_ALL :
//                    throw std::invalid_argument("Mode already set, please only set mode once");
//            }
//            else if (inputFile.empty())
//            {
//                inputFile = argv[i];
//            }
//            else
//            {
//                throw std::invalid_argument(str("Unrecognised argument ") + argv[i]);
//            }
//        }
//
//        if (inputFile.empty())
//            inputFile = "ClueDo.txt";
//
//        switch (g_mode)
//        {
//        case Mode::NONE:
//            throw std::invalid_argument("No mode chosen");
//        case Mode::HIDE_ALL:
//            std::cout << "Only obvious info will be shown during game." << std::endl;
//            break;
//        case Mode::EVENTS_ONLY:
//            std::cout << "Only events will be shown during the game." << std::endl;
//            break;
//        case Mode::SHOW_ALL:
//            std::cout << "All information will be shown." << std::endl;
//            break;
//        }
//
//        // Calling gameplay functions
//        loadCards(inputFile);
//        startingInfo();
//        playGame();
//    }
//    catch (const std::invalid_argument & ex)
//    {
//        std::cout << "Invalid argument: " << ex.what() << "\n"
//            << "Arguments are:\n"
//            << "        Mode:\n"
//            << "            -h, --hide      Don't show any info until the game is over\n"
//            << "            -e, --events    Only display events; What was asked by who\n"
//            << "            -s, --show      Display info during the game, including who has what cards\n"
//            << "\n"
//            << "        path/to/file    Path to details file containing card names."
//            << std::endl;
//    }
//    catch (const std::exception & ex)
//    {
//        std::cout << "Error: " << ex.what() << std::endl;
//        exitCode = EXIT_FAILURE;
//    }
//
//    return exitCode;
//}
