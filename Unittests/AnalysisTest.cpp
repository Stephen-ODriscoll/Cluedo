
#include "pch.h"

struct AnalysisTest : testing::Test
{
    Analysis* pAnalysis;
    std::vector<Card> cards;

    AnalysisTest() :
        pAnalysis(nullptr),
        cards(18, Card("", "")) { }

    void SetUp()
    {
        pAnalysis = new Analysis(new Player());
    }

    void TearDown()
    {
        delete pAnalysis->pPlayer;
        delete pAnalysis;

        for (auto& card : cards)
            card.pOwner = nullptr;
    }
};

TEST_F(AnalysisTest, process_has)
{
    pAnalysis->processHas(&cards[0]);

    EXPECT_NE(cards[0].pOwner, nullptr);
    EXPECT_NE(pAnalysis->has.find(&cards[0]), pAnalysis->has.end());
}

TEST_F(AnalysisTest, process_has_throws)
{
    Player* pPlayer = new Player();
    cards[0].pOwner = pPlayer;

    EXPECT_THROW(pAnalysis->processHas(&cards[0]), contradiction);
    delete pPlayer;;
}

TEST_F(AnalysisTest, process_doesnt_have)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processDoesntHave(pCards);

    // doesntHave should now be a set of the cards we just gave it to process
    EXPECT_EQ(pAnalysis->doesntHave, std::set<Card*>(pCards.begin(), pCards.end()));
}

TEST_F(AnalysisTest, process_doesnt_have_throws_after_process_has)
{
    pAnalysis->processHas(&cards[0]);
    EXPECT_THROW(pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }), contradiction);
}

TEST_F(AnalysisTest, process_doesnt_have_throws_after_process_has_either)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processHasEither(pCards);

    EXPECT_THROW(pAnalysis->processDoesntHave(pCards), contradiction);
}

TEST_F(AnalysisTest, process_doesnt_have_excludes_cards_with_known_location)
{
    Player* pPlayer = new Player();
    cards[0].pOwner = pPlayer;
    cards[1].pOwner = pPlayer;
    cards[2].pOwner = pPlayer;

    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] });

    // doesntHave should now be an empty set and avoid duplicate info.
    EXPECT_TRUE(pAnalysis->doesntHave.empty());
    delete pPlayer;
}

TEST_F(AnalysisTest, process_has_either_finds_card_when_only_one_has_unknown_location)
{
    Player* pPlayer = new Player();
    cards[0].pOwner = pPlayer;
    cards[1].pOwner = pPlayer;

    pAnalysis->processHasEither({ &cards[0], &cards[1], &cards[2] });

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[2]);
    delete pPlayer;
}

TEST_F(AnalysisTest, process_has_either_finds_card_after_process_doesnt_have)
{
    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processHasEither({ &cards[1], &cards[2], &cards[3] });

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[3]);
}

TEST_F(AnalysisTest, process_has_either_throws_after_process_doesnt_have)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processDoesntHave(pCards);

    EXPECT_THROW(pAnalysis->processHasEither(pCards), contradiction);
}

TEST_F(AnalysisTest, recheck_cards_does_nothing_when_nothing_needs_to_be_done)
{
    EXPECT_FALSE(pAnalysis->recheckCards());
}

TEST_F(AnalysisTest, recheck_cards_removes_known_cards_from_doesnt_have)
{
    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] });

    Player* pPlayer = new Player();
    cards[0].pOwner = pPlayer;
    cards[1].pOwner = pPlayer;
    cards[2].pOwner = pPlayer;

    EXPECT_FALSE(pAnalysis->recheckCards());
    EXPECT_TRUE(pAnalysis->doesntHave.empty());
    delete pPlayer;
}

TEST_F(AnalysisTest, recheck_cards_finds_cards_after_has_either)
{
    pAnalysis->processHasEither({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processHasEither({ &cards[3], &cards[4], &cards[5] });
    pAnalysis->processHasEither({ &cards[6], &cards[7], &cards[8] });

    Player* pPlayer = new Player();
    cards[0].pOwner = pPlayer;
    cards[1].pOwner = pPlayer;
    cards[3].pOwner = pPlayer;
    cards[4].pOwner = pPlayer;
    cards[6].pOwner = pPlayer;
    cards[7].pOwner = pPlayer;

    EXPECT_TRUE(pAnalysis->recheckCards());
    EXPECT_EQ(pAnalysis->has, std::set<Card*>({ &cards[2], &cards[5], &cards[8] }));
    delete pPlayer;
}
