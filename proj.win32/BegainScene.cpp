#include "BegainScene.h"
#include "LevelScene.h"
#include "PlayScene.h"

// 初始化方法（核心：添加精灵、按钮、背景、绑定事件都在这里）
bool BegainScene::init()
{
    // 调用父类初始化
    if (!Scene::init())
    {
        return false;
    }

    // ========== 你的场景逻辑写在这里 ==========
    // 示例：获取屏幕尺寸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto lc = LayerColor::create(Color4B(255, 100, 100, 255), visibleSize.width, visibleSize.height);

    this->addChild(lc, 1);

    // 标签
    auto label = Label::createWithSystemFont("Card Match Game", "Arial", 48);
	label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 3 / 4));
    this->addChild(label, 2);


    // 按钮区域
    std::string title[] = {"Continue","Start","Set","About"};
    int btn_h = 60;
    int btn_w = 200;
	int interval = 20;
    int front_size = 32;
	Color3B front_color = Color3B::WHITE;

    _functions.emplace_back([=](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            CCLOG("Continue");
            // 导入上一次游玩记录
            int lastLevel = UserDefault::getInstance()->getIntegerForKey("last_level", 1);
			auto playScene = PlayScene::create();
            playScene->setLevel(lastLevel);
            Director::getInstance()->pushScene(playScene);
        }
    });
    _functions.emplace_back([=](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            CCLOG("Start");
			// 压入一个新场景
            Director::getInstance()->pushScene(LevelScene::create());
        }
    });
    _functions.emplace_back([=](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            CCLOG("Set");
        }
    });
    _functions.emplace_back([=](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            CCLOG("About");
        }
    });

    for (int i = 0; i < 4;i++) {
        // 1. 空创建（不加载图片）
        auto btn = Button::create();

        // 2. 直接设置文字
        btn->setTitleText(title[i]);       // 文字
        btn->setTitleFontSize(front_size);          // 大小
        btn->setTitleColor(front_color); // 颜色
		btn->setContentSize(Size(btn_w, btn_h)); // 大小（如果不设置，默认是根据文字自动调整的）

        // 3. 位置
        btn->setPosition(Vec2(origin.x+visibleSize.width/2, 
            origin.y+ visibleSize.height / 2 + (btn_h + interval * 4 + btn_h)/2 - i * (btn_h+ interval)));

        // 4. 点击事件（3.x 标准回调）
        btn->addTouchEventListener(_functions[i]);

		this->addChild(btn, 2);
    }

    // 如果需要每帧更新，开启这个函数
    //this->scheduleUpdate();

    return true;
}

// 每帧更新函数（delta：距离上一帧的时间，单位秒）
void BegainScene::update(float delta)
{
    // 持续执行的逻辑：移动、计时、检测等

    CCLOG("每帧更新，delta = %f", delta);
}

// 场景销毁时调用
void BegainScene::onExit()
{
    Scene::onExit();
    // 释放自定义资源
}