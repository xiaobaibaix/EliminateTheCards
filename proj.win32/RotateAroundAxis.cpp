#include "RotateAroundAxis.h"

RotateAroundAxis::RotateAroundAxis()
    : _axis(Vec3::ZERO)
    , _angle(0.0f)
    , _callbackTriggered(false)
{
    _startQuat = Quaternion::identity();
    _endQuat = Quaternion::identity();
}

RotateAroundAxis::~RotateAroundAxis()
{}

RotateAroundAxis* RotateAroundAxis::create(float duration, const Vec3& axis, float angle, const std::function<void()>& onVerticalCallback)
{
    RotateAroundAxis* ret = new RotateAroundAxis();
    if (ret && ret->initWithDuration(duration, axis, angle, onVerticalCallback))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool RotateAroundAxis::initWithDuration(float duration, const Vec3& axis, float angle, const std::function<void()>& onVerticalCallback)
{
    if (!ActionInterval::initWithDuration(duration))
        return false;

    _axis = axis;
    _axis.normalize();
    _angle = angle;
    _onVerticalCallback = onVerticalCallback;
    _callbackTriggered = false;

    return true;
}

void RotateAroundAxis::startWithTarget(Node* target)
{
    ActionInterval::startWithTarget(target);
    if (_target)
    {
        _startQuat = _target->getRotationQuat();
        Quaternion rotQuat;
        Quaternion::createFromAxisAngle(_axis, CC_DEGREES_TO_RADIANS(_angle), &rotQuat);
        _endQuat = _startQuat * rotQuat;
        _callbackTriggered = false;
    }
}

void RotateAroundAxis::update(float time)
{
    if (!_target) return;

    Quaternion currentQuat;
    Quaternion::slerp(_startQuat, _endQuat, time, &currentQuat);
    _target->setRotationQuat(currentQuat);

    // 检测是否垂直于 XY 平面
    if (!_callbackTriggered && _onVerticalCallback && isNodeVerticalToXYPlane(_target))
    {
        isNodeVerticalToXYPlane(_target);
        _callbackTriggered = true;
        _onVerticalCallback();
    }
}

// 判断节点是否 垂直于 全局 XY 平面
bool RotateAroundAxis::isNodeVerticalToXYPlane(Node* node)
{
    // 1. 获取节点的世界矩阵
    Mat4 mat = node->getNodeToWorldTransform();

    // 2. 取出节点自身的 Z 轴方向（局部 Z 轴在世界中的朝向）
    Vec3 nodeLocalZ(mat.m[8], mat.m[9], mat.m[10]);
    nodeLocalZ.normalize();

    // 3. 全局 XY 平面的法向量 = 世界 Z 轴
    Vec3 worldZ = Vec3::UNIT_Z;

    // 4. 点积 = 0 → 两个向量垂直 → 两个平面垂直
    float dot = nodeLocalZ.dot(worldZ);

    // 5. 接近 0 就认为垂直（浮点误差容错）
    if(_clockwise)
        return fabs(dot) < 0.2;
    else 
        return fabs(dot) < 0.19;
}

RotateAroundAxis* RotateAroundAxis::clone() const
{
    return RotateAroundAxis::create(_duration, _axis, _angle, _onVerticalCallback);
}

RotateAroundAxis* RotateAroundAxis::reverse() const
{
    return RotateAroundAxis::create(_duration, _axis, -_angle, _onVerticalCallback);
}