#include "stdafx.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(Controller* pController, Game* pGame, const Player* pPlayer, const size_t stageToDisplay, QWidget* parent) :
    pController(pController),
    pGame(pGame),
    pPlayer(pPlayer),
    stageDisplayed(std::min(stageToDisplay, pPlayer->stagePresets.size())),
    QWidget(parent)
{
    ui.setupUi(this);

    for (size_t i = 1; i <= pPlayer->stagePresets.size(); ++i)
        ui.stageBox->addItem(str(i).c_str());

    ui.stageBox->setCurrentIndex(stageDisplayed - 1);

    std::vector<QComboBox*> categoryBoxes = { ui.cat1Box, ui.cat2Box, ui.cat3Box };
    auto& it1 = categoryBoxes.begin();
    for (auto& it2 = g_cards.begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
    {
        // Add cards for this category
        for (auto& item : *it2)
            (*it1)->addItem(item.name.c_str());
    }
    
    connect(ui.cat1Box, SIGNAL(textActivated(QString)), this, SLOT(cat1BoxChanged(QString)));
    connect(ui.cat2Box, SIGNAL(textActivated(QString)), this, SLOT(cat2BoxChanged(QString)));
    connect(ui.cat3Box, SIGNAL(textActivated(QString)), this, SLOT(cat3BoxChanged(QString)));
    connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));
    connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));

    resetButtonClicked();
}

void PlayerInfo::updateInfo()
{
    setWindowTitle((pPlayer->name + str("'s Info")).c_str());
    ui.renameText->setText(pPlayer->name.c_str());

    ui.cardsList->clear();
    for (const str& cardName : cardNames)
        ui.cardsList->addItem(cardName.c_str());

    if (cardNames.empty())
        ui.cardsList->addItem("No preset cards");
}

void PlayerInfo::toggleCardOwned(const str& cardName)
{
    auto it = std::find(cardNames.begin(), cardNames.end(), cardName);

    if (it == cardNames.end())
        cardNames.push_back(cardName);
    else
        cardNames.erase(it);

    updateInfo();
}

void PlayerInfo::stageBoxChanged(const QString& text)
{
    stageDisplayed = str(text.toStdString()).toull();
    resetButtonClicked();
}

void PlayerInfo::cat1BoxChanged(const QString& text) { toggleCardOwned(text.toStdString()); }
void PlayerInfo::cat2BoxChanged(const QString& text) { toggleCardOwned(text.toStdString()); }
void PlayerInfo::cat3BoxChanged(const QString& text) { toggleCardOwned(text.toStdString()); }

void PlayerInfo::resetButtonClicked()
{
    cardNames.clear();
    for (const Card* card : pPlayer->stagePresets[stageDisplayed - 1].pCardsOwned)
        cardNames.push_back(card->name);

    updateInfo();
};

void PlayerInfo::applyButtonClicked()
{
    pController->updateHasCards(pPlayer, cardNames, stageDisplayed - 1);
    
    // Error message is handled by the Controller calling the main GUI
    pController->rename(pPlayer, ui.renameText->toPlainText().toStdString());
   
    updateInfo();
};
