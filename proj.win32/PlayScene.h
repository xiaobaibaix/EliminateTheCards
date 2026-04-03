#ifndef  _PLAYSCENE_H_
#define  _PLAYSCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Card.h"
#include "CardData.h"


#include <stack> 
#include <fstream>                // 文件读写

#include "json/rapidjson.h"
#include "json/document.h"         // 核心解析类
#include "json/writer.h"          // 写入类
#include "json/stringbuffer.h"    // 字符串缓存


USING_NS_CC;
using namespace cocos2d::ui;

class PlayScene:public cocos2d::Scene
{

public:
    // 【1】静态创建方法（Cocos2d-x标准工厂模式，必须写）
    CREATE_FUNC(PlayScene);

    // 【2】初始化方法（场景核心逻辑写在这里）
    virtual bool init();

    // 【3】每帧更新函数（可选，需要持续执行逻辑时开启）
    virtual void update(float delta) override;

    // 【4】场景销毁时调用（可选，释放资源）
    virtual void onExit() override;

public:
    // 生成卡片
	void gnerateCards(int level);
    // 布置卡片
	void arrangeCards();
	// 回退操作
	void backtrack();
	// 检查是否满足胜利条件
	void checkWinCondition();
	// 检查是否满足失败条件
	void checkLoseCondition();
	// 处理卡片被点击的事件
	void onCardClicked(Card* card);
    // 设置关卡内容
    void setLevel(int level);
private:
	// 从JSON文件中读取卡牌数据
    std::vector<CardData> readJsonFile(const std::string& jsonPath);
	// 解析单个卡牌数据
    bool parseSingleCardData(const rapidjson::Value& cardObj, CardData& cardData);
	// 递归解析卡牌数据并创建卡牌对象
    bool ParingCardChilren(const std::vector<CardData>& chilren,Card* rootCard,int& deepth);
	// 布置手牌
    int arrangeCardsHandCards(Card* card, int len);

private:
    Vector<Card*> _handCards;
    Vector<Card*> _tailCards;
    std::stack<Card*> _tailStack;
    
    Node* _hand_card_node = nullptr;
    Node* _tail_card_node = nullptr;

    int _max_stack_look_size = 5;
    int _max_deepth = 0;
    int _gap = 30;
	int _offset_tail_carrd = 120;

    cocos2d::ui::Button* _backtrack_button = nullptr;
};

#endif // _CARD_DATA_H_