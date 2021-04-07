#include "stdafx.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(Game* pGame, const Player* pPlayer, const size_t stageIndexSelected, QWidget* parent) :
    pGame(pGame),
    pPlayer(pPlayer),
    stageIndex(std::min(stageIndexSelected, pPlayer->presets.size() - 1)),
    QWidget(parent)
{
    ui.setupUi(this);

    for (size_t i = 1; i <= pPlayer->presets.size(); ++i)
        ui.stageBox->addItem(str(i).c_str());

    ui.stageBox->setCurrentIndex(stageIndex);

    std::vector<QComboBox*> categoryBoxes = { ui.cat1Box, ui.cat2Box, ui.cat3Box };
    auto& it1 = categoryBoxes.begin();
    for (auto& it2 = g_categories.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
    {
        // Add cards for this category
        for (auto& item : it2->cards)
            (*it1)->addItem(item.name.c_str());
    }
    
    connect(ui.stageBox, SIGNAL(currentIndexChanged(int)), this, SLOT(stageBoxChanged(int)));
    connect(ui.numCardsBox, SIGNAL(valueChanged(int)), this, SLOT(numCardsBoxChanged(int)));
    connect(ui.cat1Box, SIGNAL(currentIndexChanged(int)), this, SLOT(cat1BoxChanged(int)));
    connect(ui.cat2Box, SIGNAL(currentIndexChanged(int)), this, SLOT(cat2BoxChanged(int)));
    connect(ui.cat3Box, SIGNAL(currentIndexChanged(int)), this, SLOT(cat3BoxChanged(int)));
    connect(ui.cat1Button, SIGNAL(clicked()), this, SLOT(cat1ButtonClicked()));
    connect(ui.cat2Button, SIGNAL(clicked()), this, SLOT(cat2ButtonClicked()));
    connect(ui.cat3Button, SIGNAL(clicked()), this, SLOT(cat3ButtonClicked()));
    connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));
    connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));

    ui.renameText->installEventFilter(this);

    resetButtonClicked();
}

void PlayerInfo::updateInfo()
{
    setWindowTitle((pPlayer->name + str("'s Info")).c_str());
    ui.renameText->setText(pPlayer->name.c_str());

    ui.numCardsBox->setValue(presets[stageIndex].numCards);

    ui.cardsList->clear();
    for (const Card* pCard : presets[stageIndex].pCardsOwned)
        ui.cardsList->addItem(pCard->name.c_str());

    if (!ui.cardsList->count())
        ui.cardsList->addItem("No preset cards");

    setButtonText(g_categories[0].cards[ui.cat1Box->currentIndex()], ui.cat1Button);
    setButtonText(g_categories[1].cards[ui.cat2Box->currentIndex()], ui.cat2Button);
    setButtonText(g_categories[2].cards[ui.cat3Box->currentIndex()], ui.cat3Button);
}

#define ADD_CARD    "Add Card"
#define REMOVE_CARD "Remove Card"
void PlayerInfo::setButtonText(Card& card, QPushButton* button)
{
    if (presets[stageIndex].pCardsOwned.find(&card) == presets[stageIndex].pCardsOwned.end())
        button->setText(ADD_CARD);
    else
        button->setText(REMOVE_CARD);
}

void PlayerInfo::toggleCardOwned(Card& card, QPushButton* button)
{
    auto it = presets[stageIndex].pCardsOwned.find(&card);
    if (it == presets[stageIndex].pCardsOwned.end())
    {
        for (size_t i = stageIndex; i != pPlayer->stages.size(); ++i)
        {
            presets[i].pCardsOwned.insert(&card);

            const int value = ui.numCardsBox->value();
            if (value && value < presets[i].pCardsOwned.size())
                ui.numCardsBox->setValue(presets[i].pCardsOwned.size());
        }

        button->setText(REMOVE_CARD);
    }
    else
    {
        // I assume someone removing a card wants it removed from all stages.
        for (size_t i = 0; i != pPlayer->stages.size(); ++i)
            presets[i].pCardsOwned.erase(&card);

        button->setText(ADD_CARD);
    }

    updateInfo();
}

void PlayerInfo::stageBoxChanged(const int index)
{
    stageIndex = size_t(index);
    updateInfo();
}

void PlayerInfo::numCardsBoxChanged(const int value)
{
    if (value && value < presets[stageIndex].pCardsOwned.size())
    {
        ui.numCardsBox->setValue((value < presets[stageIndex].numCards) ?
            0 : presets[stageIndex].pCardsOwned.size());
    }
    else
    {
        presets[stageIndex].numCards = value;
    }
}

void PlayerInfo::cat1BoxChanged(const int index) { setButtonText(g_categories[0].cards[index], ui.cat1Button); }
void PlayerInfo::cat2BoxChanged(const int index) { setButtonText(g_categories[1].cards[index], ui.cat2Button); }
void PlayerInfo::cat3BoxChanged(const int index) { setButtonText(g_categories[2].cards[index], ui.cat3Button); }

void PlayerInfo::cat1ButtonClicked() { toggleCardOwned(g_categories[0].cards[ui.cat1Box->currentIndex()], ui.cat1Button); }
void PlayerInfo::cat2ButtonClicked() { toggleCardOwned(g_categories[1].cards[ui.cat2Box->currentIndex()], ui.cat2Button); }
void PlayerInfo::cat3ButtonClicked() { toggleCardOwned(g_categories[2].cards[ui.cat3Box->currentIndex()], ui.cat3Button); }

void PlayerInfo::resetButtonClicked()
{
    presets = pPlayer->presets;
    updateInfo();
}

void PlayerInfo::applyButtonClicked()
{
    TRY
        pGame->controller.updatePresets(pPlayer, presets);
        pGame->controller.rename(pPlayer, ui.renameText->toPlainText().toStdString());
    CATCH

    pGame->refresh();
    updateInfo();
    close();
}

bool PlayerInfo::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui.renameText && event->type() == QEvent::KeyPress)
    {
        switch (static_cast<QKeyEvent*>(event)->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            applyButtonClicked();
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}
