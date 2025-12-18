#include "Cell.h"

std::pair<int, int> Hex::GetQR() const { return std::make_pair(q, r); }
bool Hex::VisibilityState() const { return IsVisible; }
bool Hex::ExplorationState() const { return IsExplored; }

QPointF Hex::GetCenter() const
{
    float x = GlobalConst::HexSize * 3.0 / 2.0 * q;
    float y = GlobalConst::HexSize * std::sqrt(3.0) * (r + q / 2.0);

    return {x,y};
}

std::vector<QPointF> Hex::GetCorners() const
{
    std::vector<QPointF> Corners;
    Corners.reserve(6);
    QPointF Center = GetCenter();

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

bool Hex::IsNeighbor(const Hex& OHex) const
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

void Hex::SetUnit(Unit* UnitTemp)
{
    Unit_ = UnitTemp;
}

Unit* Hex::GetUnit() const
{
    return Unit_;
}

bool Hex::HaveUnit() const
{
    return Unit_ != nullptr;
}

void Hex::ClearUnit()
{
    Unit_ = nullptr;
}
