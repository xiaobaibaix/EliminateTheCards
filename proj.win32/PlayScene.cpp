#include "PlayScene.h"
#include "SimpleAudioEngine.h"

bool PlayScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto _root_card_node = LayerColor::create(Color4B(255, 100, 100, 255), visibleSize.width, visibleSize.height);
	_root_card_node->setContentSize(visibleSize);
    _root_card_node->setPosition(Vec2::ZERO);
    this->addChild(_root_card_node, 0);

    _hand_card_node = LayerColor::create(Color4B(100, 100, 255, 255), visibleSize.width, visibleSize.height * 3 / 4);
    _hand_card_node->setPosition(Vec2(origin.x+20, origin.y + visibleSize.height * 1 / 4));
	_hand_card_node->setContentSize(Size(visibleSize.width-40, visibleSize.height *3/4 - 20));
    this->addChild(_hand_card_node,1);

    _tail_card_node = LayerColor::create(Color4B(100, 255, 100, 255), visibleSize.width, visibleSize.height * 1 / 4);
    _tail_card_node->setPosition(Vec2(origin.x+20,origin.y+20));
	_tail_card_node->setContentSize(Size(visibleSize.width - 40, visibleSize.height * 1 / 4 - 20));
    this->addChild(_tail_card_node,1);


    auto btn = Button::create("res/play_1.png", "res/play_2.png");

    btn->setScale(0.25f);
    btn->setTitleFontSize(24);
    btn->setTitleFontName("Arial");
    btn->setContentSize(Size(64, 64));
    btn->setPosition(Vec2(
        _tail_card_node->getPosition().x+ _tail_card_node->getContentSize().width / 2 + 200, 
        _tail_card_node->getPosition().y+ _tail_card_node->getContentSize().height / 2));
    _backtrack_button = btn;
    
    btn->addClickEventListener([=](Ref* sender) {
        CCLOG("btn clicked!");
        backtrack();
    });

    this->addChild(btn,1);

	srand(time(NULL)); // 设置随机数种子，确保每次运行结果不同
    // 如果需要每帧更新，开启这个函数
    //this->scheduleUpdate();

    return true;
}

// 每帧更新函数（delta：距离上一帧的时间，单位秒）
void PlayScene::update(float delta)
{
    // 持续执行的逻辑：移动、计时、检测等

    CCLOG("每帧更新，delta = %f", delta);
}

// 场景销毁时调用
void PlayScene::onExit()
{
    Scene::onExit();
    // 释放自定义资源
}


//生成卡片
void PlayScene::gnerateCards(int level) {
    _tailCards.clear();

    //加载关卡卡片到_handCards
    std::string str = StringUtils::format("res/config/level_config_%d.json", level);

    auto data = readJsonFile(str);

    if (!data.empty())
    {
        ParingCardChilren(data, nullptr, _max_deepth);
    }

    //生成其他的卡片，根据_hand_

    bool number[14] = { 0 };

    for (auto d:_handCards) {
        CardFaceType face = d->getFace();
        CardSuitType suit = d->getSuit();
        for (int i = 0; i < 4; i++) {
            if (suit==i) {
                continue;
            }
            else {
                auto  card = Card::create();
				card->setSuitFace((CardSuitType)i, face);
                _tailCards.pushBack(card);
                card->setScene(this);
            }
        }
        number[face] = true;
    }

    for (int i = 0; i < 13;i++) {
        if (number[i])continue;
        for (int j = 0; j < 4; j++) {
            auto  card = Card::create();
			card->setSuitFace((CardSuitType)j, (CardFaceType)i);
            _tailCards.pushBack(card);
            card->setScene(this);
        }
    }

	//打乱栈区卡片顺序
    // 打乱卡片顺序（Fisher-Yates 洗牌，完美适配 Cocos Vector）
    ssize_t total = _tailCards.size();
    for (ssize_t k = total - 1; k > 0; k--) {
        // 生成 0 ~ k 之间的随机索引
        int randomIndex = rand() % (k + 1);

        // 1. 取出当前位置的卡片
        Card* temp = _tailCards.at(k);
        // 2. 将随机位置的卡片替换到当前位置
        _tailCards.replace(k, _tailCards.at(randomIndex));
        // 3. 将临时保存的卡片替换到随机位置
        _tailCards.replace(randomIndex, temp);
    }
}


bool PlayScene::ParingCardChilren(const std::vector<CardData>& chilren, Card* rootCard, int& deepth)
{
    // 进入递归时，层级 +1
    if(!chilren.empty())deepth++;

    // 记录当前层级（用于回溯）
    int currentLevel = deepth;
    int max_deepth = deepth; 


    for (auto d : chilren) {
        Card* card = Card::create();
        card->setSuitFace(d.cardId.suit, d.cardId.face);
        card->setScene(this);

        _handCards.pushBack(card);
        card->setPosition(d.position);

        if (d.isOpen) {
            card->Front();
        }
        else {
            card->Back();
        }

        if (rootCard) {
            rootCard->_childrenCards.push_back(card);
            rootCard->_openConditions.push_back(d.cardId);
        }

        // 递归：继续往下创建子节点
        ParingCardChilren(d.children, card, deepth);

        max_deepth=std::max(max_deepth, deepth);
        // 回溯：回到当前层级
        deepth = currentLevel;
    }

    deepth = max_deepth;
    return true;
}


int PlayScene::arrangeCardsHandCards(Card* card,int len) {
    for (auto d:card->_childrenCards) {
        bool isExist = false;
        for (auto node : this->getChildren())
        {
            if (node == d)
            {
                isExist = true;
                break;
            }
        }
        if (!isExist)
        {
            this->addChild(d, len);
			d->setPosition(d->getPosition()+_hand_card_node->getPosition());
            CCLOG("addchild (%d,%d) deepth=%d", (int)d->getFace(), (int)d->getSuit(), len);
        }
        arrangeCardsHandCards(d, len - 1);
    }
    return true;
}

//布置卡片
void PlayScene::arrangeCards() {
	//将对应卡片添加到场景中
	for (auto card : _handCards) {
        if (!arrangeCardsHandCards(card, _max_deepth)) {
            CCLOGINFO("already exit");
        }

        bool isExist = false;
        for (auto node : this->getChildren())
        {
            if (node == card)
            {
                isExist = true;
                break;
            }
        }
        if (!isExist)
        {
            this->addChild(card, _max_deepth + _hand_card_node->getLocalZOrder());
            card->setPosition(card->getPosition() + _hand_card_node->getPosition());
            CCLOG("addchild (%d,%d) deepth=%d", (int)card->getFace(), (int)card->getSuit(), _max_deepth + _hand_card_node->getLocalZOrder());
        }
    }
	// 将对应卡片添加到场景中
    int size = _tail_card_node->getLocalZOrder();
	for (auto card : _tailCards) {
        this->addChild(card,_max_stack_look_size+ _tail_card_node->getLocalZOrder() -size);
        card->setPosition(Vec2(
            _tail_card_node->getPosition().x+_tail_card_node->getContentSize().width / 2 - _offset_tail_carrd + ((_max_stack_look_size-1) - size- _tail_card_node->getLocalZOrder()) * _gap,
            _tail_card_node->getPosition().y+_tail_card_node->getContentSize().height / 2));
        size++;
        if (size- _tail_card_node->getLocalZOrder() >= _max_stack_look_size)break;
    }
}


// 回退操作
void PlayScene::backtrack() {
    if (_tailStack.empty()) {
        CCLOG("Stack is nnull");
        return;
    }
    auto c = _tailStack.top();
    _tailStack.pop();
    if(!_tailStack.empty())_tailStack.top()->setVisible(true);

    auto h = c->_cardStack.top();
    c->_cardStack.pop();


    c->setPosition(h.position);
    c->setRotation(h.rotation);

    if (h.isOpen&&c->isBack()) {
        c->Front();
        CCLOG("Front");
    }
    else if(!h.isOpen&&!c->isBack()) {
        c->Back();
        CCLOG("Back");
    }
    else {
        CCLOG("other condition");
    }

    if (h.isvisable&&!c->isVisible()) {
        c->setVisible(true);
    }
    else if (!h.isvisable && c->isVisible()) {
        c->setVisible(false);
    }
    bool isexit = false;
    if (h.parent == &_handCards) {
        for (auto caard : _handCards) {
            isexit = caard == c;
        }
        if (!isexit) {
            _handCards.pushBack(c);
        }
    }
    else if (h.parent == &_tailCards) {
        for (auto caard : _tailCards) {
            isexit = caard == c;
        }
        if (!isexit) {
            for (int i = 0; i < 4;i++) {
                auto tc = _tailCards.at(i);

                auto tc_h = tc->_cardStack.top();
                tc->_cardStack.pop();

                tc->setPosition(tc_h.position);
                tc->setLocalZOrder(tc_h.showLayer);
            }
            _tailCards.insert(0,c);


        }
    }

    c->setLocalZOrder(h.showLayer);
}

// 检查是否满足胜利条件
void PlayScene::checkWinCondition() {
	if (_handCards.empty()) {
        CCLOG("You win!");
    }
}

// 处理卡片被点击的事件
void PlayScene::onCardClicked(Card* card) {
    if (!card) return;

    // 1. 判断是手牌还是牌堆
    if (_handCards.contains(card))
    {
        if (card->isBack())return;
        // 处理手牌点击逻辑：选中、移动、消除等
        if (_tailStack.empty())return;
        Card* openCardHnad = _tailStack.top();
        if (!openCardHnad)return;

		// 判断是否满足添加条件,只要openCardHnad的数值和点击的卡片数值相差1即可
		CCLOG("openCardHnad (%d,%d) card (%d,%d)", (int)openCardHnad->getFace(), (int)openCardHnad->getSuit(), (int)card->getFace(), (int)card->getSuit());
        if (abs(openCardHnad->getFace() - card->getFace()) == 1) {
            card->_cardStack.push(Card::CardHistory{
                card->getPosition(),
                card->getRotation(),
                true,
                true,
                &_handCards,
                card->getLocalZOrder()
                }
            );
            
            Vec2 card_tail_pos = openCardHnad->getPosition();
            // 满足条件，执行添加逻辑
            // 翻牌
            
            for (int i = 0; i < card->_childrenCards.size(); i++) {
                auto c_card = card->_childrenCards[i];
                c_card->_cardStack.push(Card::CardHistory{
                    c_card->getPosition(),
                    c_card->getRotation(),
                    false,
                    true,
                    &_handCards,
                    c_card->getLocalZOrder()
                    }
                );
                c_card->Front();
            }
            
            // 执行动画
            card->setLocalZOrder(999);
            card->shiftTo(1.f,
                card_tail_pos-card->getPosition(),
                0,
                [=](Card* card) {
                // 添加完成后的回调，可以在这里执行后续逻辑
					_handCards.eraseObject(card);
                    _tailStack.top()->setVisible(false);
					card->setLocalZOrder(_tailStack.top()->getLocalZOrder());
                    _tailStack.push(card);
                    card->setLocalZOrder(_tailStack.top()->getLocalZOrder());
                    CCLOG("the stack top(%d,%d)", _tailStack.top()->getFace(), _tailStack.top()->getSuit());
				});
        }
    }
    else if (_tailCards.contains(card))
    {
        if (_tailCards.front() != card) {
            return;
        }// 处理牌堆点击：发牌、翻开等

		card->_cardStack.push(Card::CardHistory{
            card->getPosition(), 
            card->getRotation(), 
            false,
            true,
            &_tailCards,
            card->getLocalZOrder()
            }
        );

        card->setLocalZOrder(999);
        card->flip3D(0.5f, Vec3(0, 1, 0), false, 
            Vec2(150, 0), 
            [=](Card* card) {
                // 翻转完成后的回调，可以在这里执行后续逻辑
                _tailCards.eraseObject(card);
				if(!_tailStack.empty())_tailStack.top()->setVisible(false);
                _tailStack.push(card);
                //刷新牌区
                //移动前三个，再添加一个显示
                int size = _tail_card_node->getLocalZOrder();
                for (auto card : _tailCards) {
                    if (size- _tail_card_node->getLocalZOrder() >= _max_stack_look_size-1) {
                        
                        card->_cardStack.push(Card::CardHistory{
                            card->getPosition(),
                            card->getRotation(),
                            false,
                            false,
                            &_tailCards,
                            card->getLocalZOrder()
                            }
                        );
                        
                        bool hasNode = false;
                        for (auto nc:this->getChildren()) {
                            if (nc == card) {
                                hasNode = true;
                                break;
                            }
                        }

                        if(!hasNode)this->addChild(card, _max_stack_look_size+ _tail_card_node->getLocalZOrder() - size);
                        card->setPosition(Vec2(
                            _tail_card_node->getPosition().x + _tail_card_node->getContentSize().width / 2 - _offset_tail_carrd + ((_max_stack_look_size - 1) - size - _tail_card_node->getLocalZOrder()) * _gap,
                            _tail_card_node->getPosition().y + _tail_card_node->getContentSize().height / 2));
                        break;
                    }

                    card->_cardStack.push(Card::CardHistory{
                        card->getPosition(),
                        card->getRotation(),
                        false,
                        true,
                        &_tailCards,
                        card->getLocalZOrder()
                        }
                    );

                    card->setPosition(card->getPosition() + Vec2(_gap, 0));
					card->setLocalZOrder(_max_stack_look_size + _tail_card_node->getLocalZOrder() - size);
                    size++;
                }
                // 压下去的全部不要渲染

			});
    }   
}

void PlayScene::setLevel(int level)
{
    //生成卡片
    gnerateCards(level);
    //布置卡片
    arrangeCards();
}

// 读取关卡配置文件
std::vector<CardData> PlayScene::readJsonFile(const std::string& jsonPath) {
    std::vector<CardData> cardDataList;

    // 1. 校验文件是否存在且非空
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(jsonPath);
    if (!FileUtils::getInstance()->isFileExist(fullPath)) {
        CCLOGERROR("cofig file is exit: %s", fullPath.c_str());
        return cardDataList;
    }

    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(fullPath);
    if (jsonStr.empty()) {
        CCLOGERROR("config is null: %s", fullPath.c_str());
        return cardDataList;
    }

    // 2. 解析JSON根节点
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (doc.HasParseError()) {
        CCLOGERROR("JSON parsing failed: %d，offset: %u",
            doc.GetParseError(), doc.GetErrorOffset());
        return cardDataList;
    }

    // 3. 校验Playfield数组
    if (!doc.HasMember("Playfield") || !doc["Playfield"].IsArray()) {
        CCLOGERROR("JSON not found Playfield，or type is error");
        return cardDataList;
    }

    const rapidjson::Value& playfieldArray = doc["Playfield"];
    cardDataList.reserve(playfieldArray.Size()); // 预分配空间提升性能

    // 4. 遍历Playfield数组，解析每张顶级卡牌
    for (rapidjson::SizeType i = 0; i < playfieldArray.Size(); i++) {
        const rapidjson::Value& cardObj = playfieldArray[i];
        CardData cardData;
        if (parseSingleCardData(cardObj, cardData)) {
            cardDataList.push_back(cardData);
        }
        else {
            CCLOGERROR(" the %d top card paring failed，jump", i + 1);
        }
    }

    CCLOGINFO("JSON paring success，all %d top card", cardDataList.size());
    return cardDataList;
}


// 递归解析单张卡牌数据（核心辅助函数）
bool PlayScene::parseSingleCardData(const rapidjson::Value& cardObj, CardData& cardData) {
    // 1. 校验并解析CardSuit/CardFace（核心ID字段）
    if (!cardObj.HasMember("CardSuit") || !cardObj["CardSuit"].IsInt() ||
        !cardObj.HasMember("CardFace") || !cardObj["CardFace"].IsInt()) {
        CCLOGERROR("card data is lose CardSuit/CardFace，or type is error");
        return false;
    }
    cardData.cardId.suit = static_cast<CardSuitType>(cardObj["CardSuit"].GetInt());
    cardData.cardId.face = static_cast<CardFaceType>(cardObj["CardFace"].GetInt());

    // 2. 解析isOpen（兼容非布尔类型，容错处理）
    if (cardObj.HasMember("isOpen") && cardObj["isOpen"].IsBool()) {
        cardData.isOpen = cardObj["isOpen"].GetBool();
    }
    else {
        CCLOGWARN("card :isOpen is erro， the default is false");
        cardData.isOpen = false;
    }

    // 3. 解析Position（坐标）
    if (!cardObj.HasMember("Position") || !cardObj["Position"].IsObject()) {
        CCLOGERROR("caed Position lose,or error");
        return false;
    }
    const rapidjson::Value& posObj = cardObj["Position"];
    if (!posObj.HasMember("x") || !posObj["x"].IsNumber() ||
        !posObj.HasMember("y") || !posObj["y"].IsNumber()) {
        CCLOGERROR("Position lose x/y，or is not number");
        return false;
    }
    cardData.position = Vec2(static_cast<float>(posObj["x"].GetDouble()),
        static_cast<float>(posObj["y"].GetDouble()));

    // 4. 解析OpenConditions（解锁条件）
    if (cardObj.HasMember("OpenConditions") && cardObj["OpenConditions"].IsObject()) {
        const rapidjson::Value& condObj = cardObj["OpenConditions"];
        if (condObj.HasMember("Cards") && condObj["Cards"].IsArray()) {
            const rapidjson::Value& condCards = condObj["Cards"];
            cardData.openConditions.reserve(condCards.Size());
            for (rapidjson::SizeType j = 0; j < condCards.Size(); j++) {
                const rapidjson::Value& condCard = condCards[j];
                if (condCard.HasMember("CardSuit") && condCard["CardSuit"].IsInt() &&
                    condCard.HasMember("CardFace") && condCard["CardFace"].IsInt()) {
                    CardData::CardId condId;
                    condId.suit = static_cast<CardSuitType>(condCard["CardSuit"].GetInt());
                    condId.face = static_cast<CardFaceType>(condCard["CardFace"].GetInt());
                    cardData.openConditions.push_back(condId);
                }
                else {
                    CCLOGWARN("open conditions in %d card port error，jump", j + 1);
                }
            }
        }
    }

    // 5. 递归解析children（子卡牌）
    if (cardObj.HasMember("children") && cardObj["children"].IsArray()) {
        const rapidjson::Value& childrenArray = cardObj["children"];
        cardData.children.reserve(childrenArray.Size());
        for (rapidjson::SizeType j = 0; j < childrenArray.Size(); j++) {
            const rapidjson::Value& childObj = childrenArray[j];
            CardData childData;
            if (parseSingleCardData(childObj, childData)) {
                cardData.children.push_back(childData);
            }
            else {
                CCLOGWARN("the %d card paring error，jump", j + 1);
            }
        }
    }

    return true;
}