#include "Brick.h"

Brick::Brick() : m_iCounter(0), m_lConterText(nullptr)
{
}

Brick* Brick::create(const std::string& filename)
{
    Brick *sprite = new (std::nothrow) Brick();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }

    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void Brick::setCounter(int value)
{
    if(!m_lConterText)
    {
        m_lConterText = LabelTTF::create();
        m_lConterText->setFontSize(40);
        m_lConterText->setPosition(_contentSize.width / 2, _contentSize.height / 2);
        addChild(m_lConterText);
    }
    m_iCounter = value;
    m_lConterText->setString(StringUtils::format("%d", m_iCounter));
    auto scale = ScaleBy::create(0.1, 2.0);
    m_lConterText->runAction(Sequence::createWithTwoActions(scale, scale->reverse()));
}

Vec2 Brick::getIntersection(const Vec2 &startPoint, const Vec2 &endPoint, Vec2 *normal)
{
    float fMinDist = std::numeric_limits<float>::max();
    Vec2 vIntersection = Vec2(fMinDist, fMinDist);
    Vec2 A = startPoint;
    Vec2 B = endPoint;

    Rect rectBox = getBoundingBox();
    for(int i = 0; i < 4; ++i)
    {
        Vec2 C = m_vBrickBorders.at(i);
        Vec2 D = m_vBrickBorders.at(i+1);

        Vec2 vPoint = Vec2::getIntersectPoint(A, B, C, D);
        float fDist = startPoint.distance(vPoint);

        if(rectBox.containsPoint(vPoint) && fDist < fMinDist)
        {
            fMinDist = fDist;
            vIntersection = vPoint;
            *normal = Vec2(C.y - D.y, D.x - C.x);
            normal->normalize();
        }

    }

    return vIntersection;
}

void Brick::setPosition(const Vec2& pos)
{
    Sprite::setPosition(pos);

    Rect box = getBoundingBox();

    Vec2 topLeft     = Vec2(box.getMinX(), box.getMaxY());
    Vec2 topRight    = Vec2(box.getMaxX(), box.getMaxY());
    Vec2 bottomRight = Vec2(box.getMaxX(), box.getMinY());
    Vec2 bottomLeft  = Vec2(box.getMinX(), box.getMinY());

    m_vBrickBorders.push_back(topLeft);
    m_vBrickBorders.push_back(topRight);
    m_vBrickBorders.push_back(bottomRight);
    m_vBrickBorders.push_back(bottomLeft);
    m_vBrickBorders.push_back(topLeft);
}


