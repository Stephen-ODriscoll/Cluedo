#include "stdafx.h"
#include "../Cluedo/Player.h"

struct PlayerTest : testing::Test
{
    void SetUp()
    {
        g_players = std::vector<Player>(6, Player());

        g_categories.clear();
        g_categories.reserve(3);
        const std::vector<size_t> categorySizes = { 6, 6, 9 };
        for (const size_t categorySize : categorySizes)
        {
            std::vector<Card> cards;
            for (size_t i = 0; i != categorySize; ++i)
                cards.emplace_back("", "", g_categories.size());
            
            g_categories.emplace_back(cards);
        }
    }
};

TEST_F(PlayerTest, process_has)
{
    g_players[0].processHas(&g_categories[0].cards[0], 0);

    EXPECT_EQ(g_categories[0].cards[0].stages[0].pOwner, &g_players[0]);
    EXPECT_EQ(*g_players[0].stages[0].has.find(&g_categories[0].cards[0]), &g_categories[0].cards[0]);
}

TEST_F(PlayerTest, process_has_throws)
{
    g_players[0].processHas(&g_categories[0].cards[0], 0);

    EXPECT_THROW(g_players[1].processHas(&g_categories[0].cards[0], 0), contradiction);
}

TEST_F(PlayerTest, process_doesnt_have)
{
    std::vector<Card*> pCards({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] });
    g_players[0].processDoesntHave(pCards, 0);

    // doesntHave should now be a set of the cards we just gave it to process
    EXPECT_EQ(g_players[0].stages[0].doesntHave, std::set<Card*>(pCards.begin(), pCards.end()));
}

TEST_F(PlayerTest, process_doesnt_have_throws_after_process_has)
{
    g_players[0].processHas(&g_categories[0].cards[0], 0);

    EXPECT_THROW(g_players[0].processDoesntHave({ &g_categories[0].cards[0] }, 0), contradiction);
}

TEST_F(PlayerTest, process_doesnt_have_throws_after_process_has_either)
{
    std::vector<Card*> pCards({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[1].cards[0] });
    g_players[0].processHasEither(pCards, 0);

    EXPECT_THROW(g_players[0].processDoesntHave(pCards, 0), contradiction);
}

TEST_F(PlayerTest, process_doesnt_have_excludes_cards_with_known_location)
{
    g_players[0].processHas(&g_categories[0].cards[0], 0);
    g_players[0].processHas(&g_categories[1].cards[0], 0);
    g_players[0].processHas(&g_categories[2].cards[0], 0);

    g_players[1].processDoesntHave({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);

    // doesntHave should now be an empty set and avoid duplicate info.
    EXPECT_TRUE(g_players[1].stages[0].doesntHave.empty());
}

TEST_F(PlayerTest, process_has_either_finds_card_when_only_one_has_unknown_location)
{
    g_players[0].processHas(&g_categories[0].cards[0], 0);
    g_players[0].processHas(&g_categories[1].cards[0], 0);

    g_players[1].processHasEither({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);

    EXPECT_EQ(*g_players[1].stages[0].has.begin(), &g_categories[2].cards[0]);
}

TEST_F(PlayerTest, process_has_either_finds_card_after_process_doesnt_have)
{
    g_players[0].processDoesntHave({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);
    g_players[0].processHasEither({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[1] }, 0);

    EXPECT_EQ(*g_players[0].stages[0].has.begin(), &g_categories[2].cards[1]);
}

TEST_F(PlayerTest, process_has_either_throws_after_process_doesnt_have)
{
    std::vector<Card*> pCards({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] });
    g_players[0].processDoesntHave(pCards, 0);

    EXPECT_THROW(g_players[0].processHasEither(pCards, 0), contradiction);
}

TEST_F(PlayerTest, process_doesnt_have_removes_known_cards_from_doesnt_have)
{
    g_players[0].processDoesntHave({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);

    g_players[1].processHas(&g_categories[0].cards[0], 0);
    g_players[1].processHas(&g_categories[1].cards[0], 0);
    g_players[1].processHas(&g_categories[2].cards[0], 0);

    g_players[0].processDoesntHave({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);
    EXPECT_TRUE(g_players[0].stages[0].doesntHave.empty());
}

TEST_F(PlayerTest, recheck_has_either_finds_cards_after_has_either)
{
    g_players[0].processHasEither({ &g_categories[0].cards[0], &g_categories[1].cards[0], &g_categories[2].cards[0] }, 0);
    g_players[0].processHasEither({ &g_categories[0].cards[1], &g_categories[1].cards[1], &g_categories[2].cards[1] }, 0);
    g_players[0].processHasEither({ &g_categories[0].cards[2], &g_categories[1].cards[2], &g_categories[2].cards[2] }, 0);

    g_players[1].processHas(&g_categories[0].cards[1], 0);
    g_players[1].processHas(&g_categories[0].cards[2], 0);
    g_players[1].processHas(&g_categories[1].cards[0], 0);
    g_players[1].processHas(&g_categories[1].cards[2], 0);
    g_players[1].processHas(&g_categories[2].cards[0], 0);
    g_players[1].processHas(&g_categories[2].cards[1], 0);

    g_players[0].recheckHasEither(0);
    EXPECT_EQ(g_players[0].stages[0].has, std::set<Card*>({ &g_categories[0].cards[0], &g_categories[1].cards[1], &g_categories[2].cards[2] }));
}
