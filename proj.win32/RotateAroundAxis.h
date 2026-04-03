#ifndef __ROTATE_AROUND_AXIS_H__
#define __ROTATE_AROUND_AXIS_H__

#include "cocos2d.h"

USING_NS_CC;

class RotateAroundAxis : public ActionInterval
{
public:
    static RotateAroundAxis* create(float duration, const Vec3& axis, float angle,const std::function<void()>& onVerticalCallback = nullptr);


    virtual void startWithTarget(Node* target) override;
    virtual void update(float time) override;
    virtual RotateAroundAxis* clone() const override;
    virtual RotateAroundAxis* reverse() const override;

	void setClockwise(bool clockwise) { _clockwise = clockwise; }

protected:
    RotateAroundAxis();
    virtual ~RotateAroundAxis();

    bool initWithDuration(float duration, const Vec3& axis, float angle, const std::function<void()>& onVerticalCallback);

private:
    bool isNodeVerticalToXYPlane(Node* node);

private:
    Vec3 _axis;
    float _angle;
    Quaternion _startQuat;
    Quaternion _endQuat;
    bool _clockwise;

    std::function<void()> _onVerticalCallback;
    bool _callbackTriggered;
};

#endif