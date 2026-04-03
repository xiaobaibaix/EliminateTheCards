#ifndef __LEVELSCENE_H__
#define __LEVELSCENE_H__

// 引入Cocos2d核心头文件
#include "cocos2d.h"
#include "ui/CocosGUI.h"

// 命名空间（简化代码书写）
USING_NS_CC;
using namespace cocos2d::ui;

// 场景类：继承自cocos2d::Scene
class LevelScene : public Scene
{
public:
    // 【1】静态创建方法（Cocos2d-x标准工厂模式，必须写）
    CREATE_FUNC(LevelScene);

    // 【2】初始化方法（场景核心逻辑写在这里）
    virtual bool init() override;

    // 【3】每帧更新函数（可选，需要持续执行逻辑时开启）
    virtual void update(float delta) override;

    // 【4】场景销毁时调用（可选，释放资源）
    virtual void onExit() override;

    // 私有成员变量（存放精灵、标签、节点等对象）
private:
    // 示例：Sprite* m_player;  自定义成员变量
};

#endif // __LEVELSCENE_H__