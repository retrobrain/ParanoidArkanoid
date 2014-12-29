#include "HelloWorldScene.h"

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B::GRAY))
    {
        return false;
    }


    Size sVSize = Director::getInstance()->getVisibleSize();
    \
    auto closeItem = MenuItemImage::create(
                "CloseNormal.png",
                "CloseSelected.png",
                CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    closeItem->setPosition(Vec2(sVSize.width - closeItem->getContentSize().width ,
                                sVSize.height - closeItem->getContentSize().height));
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 1);

    m_vBricks.reserve(BRICKS_AMOUNT);
    m_vBorderLines.reserve(BORDER_AMOUNT);

    //====================create gamefield=====================
    Vec2 vTop    = Vec2(sVSize.width/2, sVSize.height);
    Vec2 vRight  = Vec2(sVSize.width, sVSize.height/2);
    Vec2 vBottom = Vec2(sVSize.width/2, 0.0f);
    Vec2 vLeft   = Vec2( 0.0, sVSize.height/2);
    m_vBorderLines.push_back(vTop);
    m_vBorderLines.push_back(vRight);
    m_vBorderLines.push_back(vBottom);
    m_vBorderLines.push_back(vLeft);
    m_vBorderLines.push_back(vTop);
    DrawNode *gameField = DrawNode::create();
    Vec2 verts[BORDER_AMOUNT] = {vTop, vRight, vBottom, vLeft};
    gameField->drawPolygon(verts, 4, Color4F(0.93, 0.95, 0.95, 1), 1, Color4F(0.93, 0.95, 0.95, 1));
    addChild(gameField);

    //====================create ball====================
    m_sBallSprite = Sprite::create("ball.png");
    m_fBallRadius = m_sBallSprite->getBoundingBox().size.width / 2;
    srand(time(NULL));
    m_vBallPosition = Vec2(sVSize.width/4, sVSize.height/2);
    m_vBallPosition.rotate(Vec2(sVSize.width/2, sVSize.height/2), CC_DEGREES_TO_RADIANS(rand()%361));
    m_sBallSprite->setPosition(m_vBallPosition.x, m_vBallPosition.y);
    addChild(m_sBallSprite, 10);
    m_motionEffect = MotionStreak::create(0.1, 1.0, m_fBallRadius*2, Color3B(242, 199, 188), "ball_streak.png");
    addChild(m_motionEffect, 9);

    //====================create bricks=======================
    m_iHitsToWin = 0;
    float brickSize = m_fBallRadius*4;
    Vec2 vaBricksPosition[BRICKS_AMOUNT] =
    {
        Vec2(sVSize.width/2+brickSize*2, sVSize.height/2-brickSize*2),
        Vec2(sVSize.width/2+brickSize*2, sVSize.height/2+brickSize*2),
        Vec2(sVSize.width/2-brickSize*2, sVSize.height/2+brickSize*2),
        Vec2(sVSize.width/2-brickSize*2, sVSize.height/2-brickSize*2)
    };
    for(int i = 0; i < BRICKS_AMOUNT; ++i)
    {
        Brick *brickObject = Brick::create(StringUtils::format("brick_%d.png", i+1));
        brickObject->setPosition(vaBricksPosition[i]);
        brickObject->setCounter(i+1);
        addChild(brickObject);
        m_vBricks.push_back(brickObject);
        m_iHitsToWin += i+1;
    }
    m_currentBrick = -1;

    //=====get random position & direction & move to it===========
    m_vBallDirection = Vec2(rand()%200 + 100, rand()%200 + 100);
    auto randomXdirection = rand()%3-1;
    auto randomYdirection = rand()%3-1;
    if(randomXdirection) m_vBallDirection.x *= randomXdirection;
    if(randomYdirection) m_vBallDirection.y *= randomYdirection;
    moveToIntersectPoint();

    scheduleUpdate();
    return true;
}

void HelloWorld::update(float delta)
{
    m_motionEffect->setPosition(m_sBallSprite->getPosition());
}

void HelloWorld::moveToIntersectPoint()
{\

    Vec2 A = m_vBallPosition;
    Vec2 B = m_vBallDirection;
    Vec2 vNormal;

    //==========calculates intersection with borders==============
    Vec2 vIntersectPoint;
    float fMinDistance = std::numeric_limits<float>::max();
    for(int i = 0; i < BORDER_AMOUNT; ++i)
    {
        Vec2 C = m_vBorderLines.at(i);  Vec2 D = m_vBorderLines.at(i+1);

        Vec2 currentNormal = Vec2(C.y - D.y, D.x - C.x);
        currentNormal.normalize();
        currentNormal.negate();
        C += currentNormal;
        D += currentNormal;

        Vec2 vPoint = Vec2::getIntersectPoint(A, A+B, C, D);

        float fPointDirection = Vec2::dot(vPoint - A, B);
        float fDist = A.distance(vPoint);
        if(fPointDirection > 1 && fDist < fMinDistance)
        {
            fMinDistance = fDist;
            vIntersectPoint = vPoint;
            vNormal = currentNormal;
        }
    }

    //==========calculates intersection with bricks==============
    for(int i = 0; i < m_vBricks.size(); ++i)
    {
        Vec2 currentNormal = Vec2::ZERO;
        Vec2 vPoint = m_vBricks.at(i)->getIntersection(A, A+B, &currentNormal);
        float fDist = A.distance(vPoint);
        float fPointDirection = Vec2::dot(vPoint - A, B);
        if(fDist < fMinDistance && fPointDirection > 1)
        {
            m_currentBrick = i;
            fMinDistance = fDist;
            vIntersectPoint = vPoint;
            vNormal = currentNormal;
        }
    }

    vIntersectPoint += (vNormal * m_fBallRadius); //adjust the intersection point to ball size

    CallFunc *callbackMove = CallFunc::create(this, callfunc_selector(HelloWorld::moveToIntersectPoint));
    CallFunc *callbackGameState = CallFunc::create(this, callfunc_selector(HelloWorld::checkGameState));
    MoveTo *move = MoveTo::create(fMinDistance/SPEED, vIntersectPoint);
    Sequence *moveAndCallback = Sequence::create(move, callbackGameState, callbackMove, nullptr);

    m_sBallSprite->runAction(moveAndCallback);
    m_vBallPosition = vIntersectPoint;
    m_vBallDirection -= 2 * vNormal * m_vBallDirection.dot(vNormal); //calculate the reflected direction
}

void HelloWorld::checkGameState()
{
    if(m_currentBrick >= 0)
    {
        int iCounter = m_vBricks.at(m_currentBrick)->getCounter();
        m_vBricks.at(m_currentBrick)->setCounter(--iCounter);
        if(iCounter == 0)
        {
            FadeOut *aFade = FadeOut::create(0.5);
            Hide    *aHide = Hide::create();
            Sequence *hideBrick = Sequence::createWithTwoActions(aFade, aHide);
            m_vBricks.at(m_currentBrick)->runAction(hideBrick);
            m_vBricks.erase(m_vBricks.begin() + m_currentBrick);
        }
        --m_iHitsToWin;
        if(m_iHitsToWin == 0)
        {
            m_vBallDirection = Vec2::ZERO;
            m_sBallSprite->stopAllActions();
        }

        m_currentBrick = -1;
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

