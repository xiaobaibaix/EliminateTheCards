#ifndef  _CARD_DATA_H_
#define  _CARD_DATA_H_

#include "cocos2d.h"

using namespace cocos2d;

// 花色类型
enum CardSuitType
{
    CST_NONE = -1,
    CST_CLUBS,
    CST_DIAMONDS,
    CST_HEARTS,
    CST_SPADES,
    CST_NUM_CARD_SUIT_TYPES
};

// 正面类型
enum CardFaceType
{
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

struct CardData
{
    struct CardId
    {
        CardSuitType suit;
        CardFaceType face;
    };
    CardId cardId = { CardSuitType::CST_NONE, CardFaceType::CFT_NONE };
    bool isOpen = false;
    Vec2 position;
    std::vector<CardData> children;         // 子卡牌
    std::vector<CardId> openConditions;     // 解锁条件
};



#endif // _CARD_DATA_H_