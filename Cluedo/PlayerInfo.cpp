#include "stdafx.h"
#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(Controller* pController, Game* pGame, const Player* pPlayer, QWidget* parent) :
    pController(pController),
    pGame(pGame),
    pPlayer(pPlayer),
    QWidget(parent)
{
    ui.setupUi(this);

    std::vector<QComboBox*> categoryBoxes = { ui.cat1Box, ui.cat2Box, ui.cat3Box };
    auto& it1 = categoryBoxes.begin();
    for (auto& it2 = pController->cards().begin(); it1 != categoryBoxes.end(); ++it1, ++it2)
    {
        // Add cards for this category
        for (auto& item : *it2)
            (*it1)->addItem(item.name.c_str());
    }

    connect(ui.renameButton, SIGNAL(clicked()), this, SLOT(renameButtonClicked()));
    connect(ui.cat1Box, SIGNAL(currentTextChanged(QString)), this, SLOT(cat1BoxChanged(QString)));
    connect(ui.cat2Box, SIGNAL(currentTextChanged(QString)), this, SLOT(cat2BoxChanged(QString)));
    connect(ui.cat3Box, SIGNAL(currentTextChanged(QString)), this, SLOT(cat3BoxChanged(QString)));
    connect(ui.cat1Button, SIGNAL(clicked()), this, SLOT(cat1ButtonClicked()));
    connect(ui.cat2Button, SIGNAL(clicked()), this, SLOT(cat2ButtonClicked()));
    connect(ui.cat3Button, SIGNAL(clicked()), this, SLOT(cat3ButtonClicked()));

    ui.renameText->installEventFilter(this);

    updateInfo();
    cat1BoxChanged(ui.cat1Box->currentText());
    cat2BoxChanged(ui.cat2Box->currentText());
    cat3BoxChanged(ui.cat3Box->currentText());
}

void PlayerInfo::updateInfo()
{
    setWindowTitle((pPlayer->name + str("'s Info")).c_str());
    ui.renameText->setText(pPlayer->name.c_str());

    ui.cardsList->clear();
    for (const Card* card : pPlayer->pCardsOwned)
        ui.cardsList->addItem(card->name.c_str());

    if (ui.cardsList->count() == 0)
        ui.cardsList->addItem("No preset cards");
}

void PlayerInfo::renameButtonClicked()
{
    // Error message is handled by the Controller calling the main GUI
    if (!pController->rename(pPlayer, ui.renameText->toPlainText().toStdString()))
        return;

    updateInfo();
}

bool PlayerInfo::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui.renameText && event->type() == QEvent::KeyPress)
    {
        int key = static_cast<QKeyEvent*>(event)->key();
        if (key == Qt::Key_Enter || key == Qt::Key_Return)
        {
            renameButtonClicked();
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}

#define ADD_CARD "Add Card"
#define REMOVE_CARD "Remove Card"
void PlayerInfo::setCardButtonText(const str& cardName, QPushButton* pButton)
{
    auto it = std::find_if(pPlayer->pCardsOwned.begin(), pPlayer->pCardsOwned.end(),
        [cardName](const Card* pCard) { return pCard->name == cardName; });

    if (it == pPlayer->pCardsOwned.end())
        pButton->setText(ADD_CARD);
    else
        pButton->setText(REMOVE_CARD);
}

void PlayerInfo::addRemoveCard(const str& cardName, QPushButton* pButton)
{
    if (pButton->text() == ADD_CARD)
    {
        pController->hasCard(pPlayer, cardName);
        pButton->setText(REMOVE_CARD);
    }
    else if (pButton->text() == REMOVE_CARD)
    {
        pController->removeHasCard(pPlayer, cardName);
        pButton->setText(ADD_CARD);
    }

    updateInfo();
}

void PlayerInfo::cat1BoxChanged(const QString& text) { setCardButtonText(text.toStdString(), ui.cat1Button); }
void PlayerInfo::cat2BoxChanged(const QString& text) { setCardButtonText(text.toStdString(), ui.cat2Button); }
void PlayerInfo::cat3BoxChanged(const QString& text) { setCardButtonText(text.toStdString(), ui.cat3Button); }

void PlayerInfo::cat1ButtonClicked() { addRemoveCard(ui.cat1Box->currentText().toStdString(), ui.cat1Button); }
void PlayerInfo::cat2ButtonClicked() { addRemoveCard(ui.cat2Box->currentText().toStdString(), ui.cat2Button); }
void PlayerInfo::cat3ButtonClicked() { addRemoveCard(ui.cat3Box->currentText().toStdString(), ui.cat3Button); }
