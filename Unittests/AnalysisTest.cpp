
#include "pch.h"

struct AnalysisTest : testing::Test
{
    Analysis* pAnalysis;
    std::vector<Card> cards;
    std::set<Card*> possibleCards;

    AnalysisTest() :
        pAnalysis(nullptr),
        cards(18, Card("", "")) { }

    void SetUp()
    {
        pAnalysis = new Analysis(new Player());

        for (auto& card : cards)
            possibleCards.insert(&card);
    }

    void TearDown()
    {
        delete pAnalysis->pPlayer;
        delete pAnalysis;

        for (auto& card : cards)
            card.pOwner = nullptr;

        possibleCards.clear();
    }
};

TEST_F(AnalysisTest, process_has)
{
    pAnalysis->processHas(&cards[0], possibleCards);

    EXPECT_NE(cards[0].pOwner, nullptr);
    EXPECT_EQ(possibleCards.find(&cards[0]), possibleCards.end());
    EXPECT_NE(pAnalysis->has.find(&cards[0]), pAnalysis->has.end());
}

TEST_F(AnalysisTest, process_has_throws)
{
    cards[0].pOwner = new Player();

    EXPECT_THROW(pAnalysis->processHas(&cards[0], possibleCards), contradiction);

    delete cards[0].pOwner;
}

TEST_F(AnalysisTest, process_doesnt_have)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pAnalysis->processDoesntHave(pCards, possibleCards);

    // doesntHave should now be a set of the cards we just gave it to process
    EXPECT_EQ(pAnalysis->doesntHave, std::set<Card*>(pCards.begin(), pCards.end()));
}

TEST_F(AnalysisTest, process_doesnt_have_throws_after_process_has)
{
    pAnalysis->processHas(&cards[0], possibleCards);
    EXPECT_THROW(pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }, possibleCards), contradiction);
}

TEST_F(AnalysisTest, process_doesnt_have_excludes_cards_not_in_possible_cards)
{
    possibleCards.erase(&cards[0]);
    possibleCards.erase(&cards[1]);
    possibleCards.erase(&cards[2]);

    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }, possibleCards);

    // doesntHave should now be a set of the cards we just gave it to process
    EXPECT_TRUE(pAnalysis->doesntHave.empty());
}

TEST_F(AnalysisTest, process_has_either_finds_card_when_only_one_is_in_possible_cards)
{
    possibleCards.erase(&cards[0]);
    possibleCards.erase(&cards[1]);

    pAnalysis->processHasEither({ &cards[0], &cards[1], &cards[2] }, possibleCards);

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[2]);
}

TEST_F(AnalysisTest, process_has_either_finds_card_after_process_doesnt_have)
{
    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }, possibleCards);
    pAnalysis->processHasEither({ &cards[1], &cards[2], &cards[3] }, possibleCards);

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[3]);
}

TEST_F(AnalysisTest, recheck_cards_does_nothing_when_nothing_needs_to_be_done)
{
    EXPECT_FALSE(pAnalysis->recheckCards(possibleCards));
}

TEST_F(AnalysisTest, recheck_cards_removes_known_cards_from_doesnt_have)
{
    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }, possibleCards);

    possibleCards.erase(&cards[0]);
    possibleCards.erase(&cards[1]);
    possibleCards.erase(&cards[2]);

    EXPECT_FALSE(pAnalysis->recheckCards(possibleCards));
    EXPECT_TRUE(pAnalysis->doesntHave.empty());
}

TEST_F(AnalysisTest, recheck_cards_finds_cards_after_has_either)
{
    pAnalysis->processHasEither({ &cards[0], &cards[1], &cards[2] }, possibleCards);
    pAnalysis->processHasEither({ &cards[3], &cards[4], &cards[5] }, possibleCards);
    pAnalysis->processHasEither({ &cards[6], &cards[7], &cards[8] }, possibleCards);

    possibleCards.erase(&cards[1]);
    possibleCards.erase(&cards[2]);
    possibleCards.erase(&cards[4]);
    possibleCards.erase(&cards[5]);
    possibleCards.erase(&cards[7]);
    possibleCards.erase(&cards[8]);

    EXPECT_TRUE(pAnalysis->recheckCards(possibleCards));
    EXPECT_EQ(pAnalysis->has, std::set<Card*>({ &cards[0], &cards[3], &cards[6] }));
}
