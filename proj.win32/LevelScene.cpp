#include "LevelScene.h"

// 初始化方法（核心：添加精灵、按钮、背景、绑定事件都在这里）
bool LevelScene::init()
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


    // 如果需要每帧更新，开启这个函数
    //this->scheduleUpdate();

    return true;
}

// 每帧更新函数（delta：距离上一帧的时间，单位秒）
void LevelScene::update(float delta)
{
    // 持续执行的逻辑：移动、计时、检测等

    CCLOG("每帧更新，delta = %f", delta);
}

// 场景销毁时调用
void LevelScene::onExit()
{
    Scene::onExit();
    // 释放自定义资源
}