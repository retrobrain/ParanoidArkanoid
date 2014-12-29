#ifndef __BRICK_H__
#define __BRICK_H__

#include "cocos2d.h"
USING_NS_CC;

class Brick : public Sprite
{
public:
    static Brick* create(const std::string& filename);

    void setCounter(int value);
    int getCounter() const {return m_iCounter;}

    //call this
    Vec2 getIntersection(const Vec2 &startPoint, const Vec2 &endPoint, Vec2 *normal);

    //overrided sprite method, needed to setup object rect automaticly
    virtual void setPosition(const Vec2& pos);

private:
    Brick();
    int       m_iCounter     = NULL;
    LabelTTF *m_lConterText  = nullptr;
    std::vector<Vec2>m_vBrickBorders;
};

#endif // __BRICK_H__
