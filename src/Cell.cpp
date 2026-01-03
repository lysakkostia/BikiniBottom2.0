#include "Cell.h"

std::pair<int, int> Hex::getQR() const { return std::make_pair(q, r); }
bool Hex::visibilityState() const { return isVisible; }
bool Hex::explorationState() const { return isExplored; }

QPointF Hex::getCenter() const
{
    float x = GlobalConst::HexSize * 3.0 / 2.0 * q;
    float y = GlobalConst::HexSize * std::sqrt(3.0) * (r + q / 2.0);

    return {x,y};
}

std::vector<QPointF> Hex::getCorners() const
{
    std::vector<QPointF> Corners;
    Corners.reserve(6);
    QPointF Center = getCenter();

    for(int i = 0; i < 6; i++)
    {
        float AngleDeg = 60.0 * i;
        float AngleRad = AngleDeg * M_PI / 180.0;
        float x = Center.x() + GlobalConst::HexSize * std::cos(AngleRad);
        float y = Center.y() + GlobalConst::HexSize * std::sin(AngleRad);
        Corners.emplace_back(x,y);
    }
    return Corners;
}

bool Hex::isNeighbor(const Hex& OHex) const
{
    int dq = OHex.q - q;
    int dr = OHex.r - r;
    static const std::vector<std::pair<int, int>> Directions = {
        {1,0},{1,-1},{0,-1},
        {-1,0},{-1,1},{0,1}
    };

    for(const auto& Dir : Directions)
    {
        if(dq == Dir.first && dr == Dir.second)
            return true;
    }
    return false;
}

void Hex::setUnit(Unit* UnitTemp)
{
    unit = UnitTemp;
}

Unit* Hex::getUnit() const
{
    return unit;
}

bool Hex::haveUnit() const
{
    return unit != nullptr;
}

void Hex::clearUnit()
{
    unit = nullptr;
}
