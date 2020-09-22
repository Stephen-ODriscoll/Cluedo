
#include "pch.h"

struct StageTest : testing::Test
{
    Stage* pStage;
    std::vector<Card> cards;

    StageTest() :
        pStage(nullptr),
        cards(18, Card("", "")) { }

    void SetUp()
    {
        pStage = new Stage(new Player(), 0);
    }

    void TearDown()
    {
        delete pStage->pPlayer;
        delete pStage;

        for (auto& card : cards)
            card.pOwners[0] = nullptr;
    }
};

TEST_F(StageTest, process_has)
{
    pStage->processHas(&cards[0]);

    EXPECT_NE(cards[0].pOwners[0], nullptr);
    EXPECT_NE(pStage->has.find(&cards[0]), pStage->has.end());
}

TEST_F(StageTest, process_has_throws)
{
    Player* pPlayer = new Player();
    cards[0].pOwners[0] = pPlayer;

    EXPECT_THROW(pStage->processHas(&cards[0]), contradiction);
    delete pPlayer;;
}

TEST_F(StageTest, process_doesnt_have)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pStage->processDoesntHave(pCards);

    // doesntHave should now be a set of the cards we just gave it to process
    EXPECT_EQ(pStage->doesntHave, std::set<Card*>(pCards.begin(), pCards.end()));
}

TEST_F(StageTest, process_doesnt_have_throws_after_process_has)
{
    pStage->processHas(&cards[0]);
    EXPECT_THROW(pStage->processDoesntHave({ &cards[0], &cards[1], &cards[2] }), contradiction);
}

TEST_F(StageTest, process_doesnt_have_throws_after_process_has_either)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pStage->processHasEither(pCards);

    EXPECT_THROW(pStage->processDoesntHave(pCards), contradiction);
}

TEST_F(StageTest, process_doesnt_have_excludes_cards_with_known_location)
{
    Player* pPlayer = new Player();
    cards[0].pOwners[0] = pPlayer;
    cards[1].pOwners[0] = pPlayer;
    cards[2].pOwners[0] = pPlayer;

    pStage->processDoesntHave({ &cards[0], &cards[1], &cards[2] });

    // doesntHave should now be an empty set and avoid duplicate info.
    EXPECT_TRUE(pStage->doesntHave.empty());
    delete pPlayer;
}

TEST_F(StageTest, process_has_either_finds_card_when_only_one_has_unknown_location)
{
    Player* pPlayer = new Player();
    cards[0].pOwners[0] = pPlayer;
    cards[1].pOwners[0] = pPlayer;

    pStage->processHasEither({ &cards[0], &cards[1], &cards[2] });

    EXPECT_EQ(*pStage->has.begin(), &cards[2]);
    delete pPlayer;
}

TEST_F(StageTest, process_has_either_finds_card_after_process_doesnt_have)
{
    pStage->processDoesntHave({ &cards[0], &cards[1], &cards[2] });
    pStage->processHasEither({ &cards[1], &cards[2], &cards[3] });

    EXPECT_EQ(*pStage->has.begin(), &cards[3]);
}

TEST_F(StageTest, process_has_either_throws_after_process_doesnt_have)
{
    std::vector<Card*> pCards({ &cards[0], &cards[1], &cards[2] });
    pStage->processDoesntHave(pCards);

    EXPECT_THROW(pStage->processHasEither(pCards), contradiction);
}

TEST_F(StageTest, recheck_cards_does_nothing_when_nothing_needs_to_be_done)
{
    EXPECT_FALSE(pStage->recheckCards());
}

TEST_F(StageTest, recheck_cards_removes_known_cards_from_doesnt_have)
{
    pStage->processDoesntHave({ &cards[0], &cards[1], &cards[2] });

    Player* pPlayer = new Player();
    cards[0].pOwners[0] = pPlayer;
    cards[1].pOwners[0] = pPlayer;
    cards[2].pOwners[0] = pPlayer;

    EXPECT_FALSE(pStage->recheckCards());
    EXPECT_TRUE(pStage->doesntHave.empty());
    delete pPlayer;
}

TEST_F(StageTest, recheck_cards_finds_cards_after_has_either)
{
    pStage->processHasEither({ &cards[0], &cards[1], &cards[2] });
    pStage->processHasEither({ &cards[3], &cards[4], &cards[5] });
    pStage->processHasEither({ &cards[6], &cards[7], &cards[8] });

    Player* pPlayer = new Player();
    cards[0].pOwners[0] = pPlayer;
    cards[1].pOwners[0] = pPlayer;
    cards[3].pOwners[0] = pPlayer;
    cards[4].pOwners[0] = pPlayer;
    cards[6].pOwners[0] = pPlayer;
    cards[7].pOwners[0] = pPlayer;

    EXPECT_TRUE(pStage->recheckCards());
    EXPECT_EQ(pStage->has, std::set<Card*>({ &cards[2], &cards[5], &cards[8] }));
    delete pPlayer;
}
