
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

TEST_F(AnalysisTest, process_has_either_finds_card_when_only_one_is_in_possible_cards)
{
    possibleCards.erase(&cards[0]);
    possibleCards.erase(&cards[1]);

    pAnalysis->processHasEither({ &cards[0], &cards[1], &cards[2] }, possibleCards);

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[2]);
}

TEST_F(AnalysisTest, process_has_either_finds_card_after_doesnt_have_rules_out_others)
{
    pAnalysis->processDoesntHave({ &cards[0], &cards[1], &cards[2] }, possibleCards);
    pAnalysis->processHasEither({ &cards[1], &cards[2], &cards[3] }, possibleCards);

    EXPECT_EQ(*pAnalysis->has.begin(), &cards[3]);
}
