#include "Card.h"
#include "RotateAroundAxis.h"
#include "PlayScene.h"

bool Card::init()
{
    if (!Node::init())
        return false;

    _suit = CardSuitType::CST_NONE;
    _face = CardFaceType::CFT_NONE;
    _isSelected = false;
    _MaxScal = 1.2;
    _MaxZ = 50;

    // 存卡片实体
    _cardNode= Node::create();

	// 阴影投影
    _shadowProjection = Sprite::create("res/card_shadow.png");
    _shadowProjection->setOpacity(100);
    _shadowProjection->setVisible(true);

	_cardNode->setContentSize(_shadowProjection->getContentSize());
    _cardNode->setPosition(Vec2::ZERO);
    _shadowProjection->setPosition(Vec2::ZERO);

    this->addChild(_cardNode, 1);
    this->addChild(_shadowProjection, 0); // 最底层
    this->setContentSize(_shadowProjection->getContentSize());

	// 牌前面和后面节点
    _frontNode = Node::create();
    _cardNode->addChild(_frontNode, 1);
    _backNode = Node::create();
    _cardNode->addChild(_backNode, 1);

	// 默认背面朝上
    if (_frontNode) {
        _frontNode->setVisible(false);
        _frontNode->setRotation3D(Vec3(0, 180, 0));
    }
    if (_backNode)
        _backNode->setVisible(true);
    _is_back = true;

	// 创建触摸监听器
    auto listener = EventListenerTouchOneByOne::create();
    // 吞噬触摸
    listener->setSwallowTouches(true);
    // 绑定回调
    listener->onTouchBegan = CC_CALLBACK_2(Card::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(Card::onTouchEnded, this);
    // 注册到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void Card::setSuitFace(CardSuitType suit, CardFaceType face) {
	_suit = suit;
	_face = face;

    // 初始化正反面UI
    initCardFrontUI();
    initCardBackUI();
}

void Card::initCardFrontUI()
{
    // 正面底图
    auto bgSprite = Sprite::create("res/card_face.png");
	bgSprite->setPosition(Vec2::ZERO);
    _frontNode->addChild(bgSprite, 0);

    const int offset = 20;
	const int big_offset = 20;
    int w = this->getContentSize().width;
    int h = this->getContentSize().height;

    // 花色
	auto sname = getSuitSpriteName();
    auto sprename = std::string("res/suits/");
    if (!sname.empty()) {
        auto suitSprite = Sprite::create(sprename+sname);
        suitSprite->setPosition(w / 2 - offset,h / 2 - offset);
		_frontNode->addChild(suitSprite, 2);
    }

    // 大小号点数
	auto bname = getFaceSpriteName();
    auto bprename = std::string("res/number/");
    if (!bname.empty()) {
        auto bigFaceSprite = Sprite::create(bprename +"big_" + bname);
        bigFaceSprite->setPosition(Vec2(0,-big_offset));
        _frontNode->addChild(bigFaceSprite, 2);

        auto smallFaceSprite = Sprite::create(bprename + "small_" + bname);
        smallFaceSprite->setPosition(Vec2(-w / 2 + offset,h / 2 - offset));
        _frontNode->addChild(smallFaceSprite, 2);
    }
}

void Card::initCardBackUI(){
    // 背面
    auto _bhSprite = Sprite::create("res/card_back.png");
	_bhSprite->setPosition(Vec2::ZERO);
    _backNode->addChild(_bhSprite, 2);
}


// 用于动画所以不需要旋转setRotation3D
void Card::turn() {
    _is_back = !_is_back;
    if (_frontNode)
        _frontNode->setVisible(!_is_back);
    if (_backNode)
        _backNode->setVisible(_is_back);
}

void Card::Back()
{
    if (_is_back) {
        CCLOG("is_back");
        return;
    }
    if (_frontNode) {
        _frontNode->setVisible(false);
        if (_frontNode->getRotation3D().y == 180) {
            CCLOG("back t 0");
            _frontNode->setRotation3D(Vec3(0, 0, 0));
        }
        else {
            CCLOG("back t 180");
            _frontNode->setRotation3D(Vec3(0, 180, 0));
        }
    }
    if (_backNode)
        _backNode->setVisible(true);
    _is_back = true;
}

void Card::Recover()
{
    if (_is_back)return;
    if (_frontNode) {
        _frontNode->setVisible(false);
        _frontNode->setRotation3D(Vec3(0, 0, 0));
    }
    if (_backNode)
        _backNode->setVisible(true);
    _is_back = true;
}

void Card::Front() {
	if (!_is_back)return;
    if (_frontNode) {
        _frontNode->setVisible(true);
        _frontNode->setRotation3D(Vec3(0, 0, 0));
    }
    if (_backNode)
        _backNode->setVisible(false);
    _is_back = false;
}

void Card::flip3D(float duration, const cocos2d::Vec3& axis, bool clockwise, const cocos2d::Vec2& pos , const std::function<void(Card*)>& onComplete)
{
    if (_is_flipping) return;
    this->stopAllActions();

    const float angle = clockwise ? 180.f : -180.0f;

    // 三维旋转
    auto rotateAction = RotateAroundAxis::create(duration, axis, angle, [this]() {
        turn();
    });
    rotateAction->setClockwise(clockwise);
    auto onCompleteAction = CallFunc::create([this, pos, onComplete]() {
        if (onComplete) onComplete(this);
		_is_flipping = false;
		});
    auto seq = Sequence::create(rotateAction, onCompleteAction,nullptr);

    //变大变小的动画
    auto sbig = ScaleTo::create(duration / 2, _cardNode->getScale()* _MaxScal);
    auto smal = ScaleTo::create(duration / 2, _cardNode->getScale());
    auto seqs = Sequence::create(sbig, smal, nullptr);

	//模拟z轴运动效果
    auto movup = MoveBy::create(duration/2, Vec2(-_MaxZ, _MaxZ));
    auto movdown = MoveBy::create(duration/2, Vec2(_MaxZ, -_MaxZ));
    auto seqm = Sequence::create(movup, movdown, nullptr);

    //同步动作
	auto together = Spawn::create(seq, seqs, seqm, nullptr);
    _cardNode->runAction(together);

    //阴影同步形成
    auto rotateAction_sha = RotateAroundAxis::create(duration, axis, angle,nullptr);
    _shadowProjection->runAction(rotateAction_sha);

    // 整体的移动
    auto allMove = MoveBy::create(duration, pos);
    this->runAction(allMove);

    _is_flipping = true;
}

void Card::shiftTo(float duration, const cocos2d::Vec2& pos,float angle, const std::function<void(Card*)>& onComplete) {
    if (_is_flipping) return;
    this->stopAllActions();

    auto sbig = ScaleTo::create(duration / 4, this->_cardNode->getScale() * _MaxScal);
    auto smal = ScaleTo::create(duration / 4, this->_cardNode->getScale());
	auto delay = DelayTime::create(duration / 2);
    auto seqs = Sequence::create(sbig, delay, smal, nullptr);

    auto movup = MoveBy::create(duration / 4, Vec2(-_MaxZ, _MaxZ));
    auto movdown = MoveBy::create(duration / 4, Vec2(_MaxZ, -_MaxZ));
    auto seqm = Sequence::create(movup, delay, movdown, nullptr);

    auto together_sha = Spawn::create(seqs, seqm, nullptr);

	_cardNode->runAction(together_sha);

    auto mov = MoveBy::create(duration, pos);
	auto rot = RotateBy::create(duration, Vec3(0, 0, angle));

    auto onCompleteAction = CallFunc::create([this, onComplete]() {
        if (onComplete)onComplete(this);
        _is_flipping = false;
        });

    auto together_rot = Spawn::create(mov, rot, nullptr);
    auto seq = Sequence::create(together_rot, onCompleteAction, nullptr);

    this->runAction(seq);
    
    _is_flipping = true;
}

void Card::translateTo(float duration, const cocos2d::Vec2& pos, const std::function<void(Card*)>& onComplete) {
    auto movup = MoveBy::create(duration, pos);
    auto onCompleteAction = CallFunc::create([this, onComplete]() {
        if (onComplete)onComplete(this);
        _is_flipping = false;
        });
    auto seqs = Sequence::create(movup, onCompleteAction, nullptr);
    _is_flipping = true;
	this->runAction(seqs);
}

bool Card::isRedSuit() const
{
    return _suit == CST_HEARTS || _suit == CST_DIAMONDS;
}

std::string Card::getSuitSpriteName() const
{
	static const char* suitNames[] = { "club", "diamond", "heart", "spade" };
    if(_suit<0||_suit>3)return std::string();
    return StringUtils::format("%s.png", suitNames[(int)_suit]);
}


std::string Card::getFaceSpriteName() const
{
	static const char* faceNames[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    if (_face < 0 || _face > 12)return std::string();
    return StringUtils::format("%s_%s.png", isRedSuit()?"red" : "black", faceNames[(int)_face]);
}

// 触摸开始（必须返回 bool：true 表示接收后续事件）
bool Card::onTouchBegan(Touch* touch, Event* event)
{
    if (!isTouchInCard(touch))
        return false; // 没点中，不处理

    _isSelected = true;

    // 关键：把当前 Card 实例传给父场景（PlayScene）
    if (auto scene = dynamic_cast<PlayScene*>(_parentScene))
    {
        scene->onCardClicked(this);
    }

    return true; // 接收 onTouchEnded
}

void Card::onTouchEnded(Touch* touch, Event* event)
{
    // 恢复状态
    _isSelected = false;
}

// 内部：判断触摸点是否在卡片矩形内
bool Card::isTouchInCard(Touch* touch)
{
    // 世界坐标 触摸点
    Vec2 touchPoint = touch->getLocation();

    Vec2 worldPos = this->convertToWorldSpace(Vec2::ZERO);
    // 3. 计算真实大小
    Size size = this->getContentSize();
    float w = size.width * this->getScaleX();
    float h = size.height * this->getScaleY();

    // 4. 构造世界矩形（锚点默认中心）
    Rect worldRect(
        worldPos.x - w / 2,   // 中心往左挪一半宽度
        worldPos.y - h / 2,   // 中心往下挪一半高度
        w,
        h
    );

    // 5. 判断点击是否在矩形内
    return worldRect.containsPoint(touchPoint);
}
