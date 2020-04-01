#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>


enum class Mode
{
    NONE,
    SINGLE,
    GROUP
};


struct Card
{
    const std::string name;
    int belongsToPlayer;

    Card(const std::string name) : name(name), belongsToPlayer(0) { }
};


namespace fs = std::filesystem;


std::vector<std::vector<Card>> loadCards(const fs::path& inputFile)
{
    std::ifstream load(inputFile);

    if (!load.is_open())
    {
        throw std::invalid_argument(std::string("Failed to load file ") + inputFile.string());
    }

    std::string line;
    std::vector<std::vector<Card>> cards { std::vector<Card>() };
    for (uint32_t category = 0; std::getline(load, line);)
    {
        if (line.empty())
        {
            if (!cards[category].empty())
            {
                cards.push_back(std::vector<Card>());
                ++category;
            }

            continue;
        }

        cards[category].push_back(line);
    }

    load.close();
    return std::move(cards);
}


void preGameKnowledge(Mode mode, std::vector<std::vector<Card>>& cards)
{

}


void begin(std::vector<std::vector<Card>> &cards)
{
    while (true)
    {

    }
}


int main(int argc, char* argv[])
{
    auto exitCode = EXIT_SUCCESS;

    try
    {
        Mode mode = Mode::NONE;
        fs::path inputFile("ClueDo.txt");
        for (int i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--single"))
            {
                if (mode != Mode::NONE)
                {
                    throw std::invalid_argument("Mode already set, please use --single/--group once");
                }

                mode = Mode::SINGLE;
            }
            else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--group"))
            {
                if (mode != Mode::NONE)
                {
                    throw std::invalid_argument("Mode already set, please use --single/--group once");
                }

                mode = Mode::GROUP;
            }
            else if (inputFile.empty())
            {
                inputFile = argv[i];

                if (!fs::exists(inputFile))
                {
                    throw std::invalid_argument(std::string("Argument isn't a valid path - ") + argv[i]);
                }
            }
            else
            {
                throw std::invalid_argument(std::string("Unrecognised argument ") + argv[i]);
            }
        }

        auto cards = loadCards(inputFile);
        preGameKnowledge(mode, cards);
        begin(cards);
    }
    catch (const std::invalid_argument & ex)
    {
        std::cout   << "Invalid argument: " << ex.what() << "\n"
                    << "Arguments are:\n"
                    << "        -s, --single    Single mode, only knows about 1 players cards\n"
                    << "        -g, --group     Group mode, knows about all players cards\n"
                    << "        path/to/file    Path to details file containing card names"
                    << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }


    return exitCode;
}