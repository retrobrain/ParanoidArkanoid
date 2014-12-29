#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Brick.h"
USING_NS_CC;

const float SPEED   = 240.0;
const int   BORDER_AMOUNT = 4;
const int   BRICKS_AMOUNT = 4;

class HelloWorld : public cocos2d::LayerColor
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Ref* pSender);

    //used only for MotionStreak position update
    virtual void update(float delta);

    //calculates and moves to target point creating a callback to this func on complete
    void moveToIntersectPoint();

    //sets briks labels, hides bricks
    void checkGameState();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    Vec2          m_vBallPosition;
    Vec2          m_vBallDirection;
    float         m_fBallRadius;
    Sprite       *m_sBallSprite;
    MotionStreak *m_motionEffect;
    int           m_currentBrick;
    int           m_iHitsToWin;

    DrawNode *m_lineTrack;

    std::vector<Vec2>m_vBorderLines;
    std::vector<Brick*>m_vBricks;
};

#endif // __HELLOWORLD_SCENE_H__
