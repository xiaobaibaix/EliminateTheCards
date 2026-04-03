#ifndef  _CARD_H_
#define  _CARD_H_

#include "cocos2d.h"

#include "CardData.h"

#include <string>
#include <functional>

USING_NS_CC;
using namespace cocos2d;



class Card : public cocos2d::Node
{
public:
    struct CardHistory {
        Vec2 position;  //位置
        float rotation; //角度
        bool isOpen;    //是否已经打开
        bool isvisable;     //是否可见
        Vector<Card*>* parent;  //属于哪一个容器
        int showLayer; // 显示层级
    };
    // 1. 自动生成 create()）
    CREATE_FUNC(Card);
    // 2. init() 函数（重写父类）
    virtual bool init() override;

public:
    CC_SYNTHESIZE_READONLY(CardSuitType, _suit, Suit);
    CC_SYNTHESIZE_READONLY(CardFaceType, _face, Face);
    CC_SYNTHESIZE(bool, _isSelected, IsSelected);
    CC_SYNTHESIZE(float, _MaxScal, MaxScal);
    CC_SYNTHESIZE(float, _MaxZ, MaxZ);


    void setSuitFace(CardSuitType suit, CardFaceType face);


public:
    void Back();
    void Recover();
    void Front();
    void turn();;
    bool isBack() const { return _is_back != 0; }

    void flip3D(float duration, const cocos2d::Vec3& axis, bool clockwise, const cocos2d::Vec2& pos ,const std::function<void(Card*)>& onComplete = nullptr);
	void shiftTo(float duration, const cocos2d::Vec2& pos, float angle, const std::function<void(Card*)>& onComplete = nullptr);
    void translateTo(float duration, const cocos2d::Vec2& pos, const std::function<void(Card*)>& onComplete = nullptr);


    std::string getSuitSpriteName() const;
    std::string getFaceSpriteName() const;

    bool isRedSuit() const;
	bool isActionInProgress() const { return _is_flipping; }

    // 触摸回调（必须是 bool 返回值，3.x 规范）
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);

    // 提供给外部判断用的属性
    void setScene(Node* scene) {_parentScene = scene;}

private:
    void initCardFrontUI();
    void initCardBackUI();

    // 内部判断：触摸点是否在卡片范围内
    bool isTouchInCard(Touch* touch);
private:
    int _is_back = true;

	Node* _cardNode = nullptr;
    // 正面精灵
    Node* _frontNode = nullptr;
    // 背面精灵
    Node* _backNode = nullptr;

    Sprite* _shadowProjection;

    // 翻转状态
    bool _is_flipping = false;

	Node* _parentScene = nullptr;
public:
	std::vector<Card* > _childrenCards;
    std::vector<CardData::CardId> _openConditions;     // 解锁条件

    std::stack<CardHistory> _cardStack;
};

#endif