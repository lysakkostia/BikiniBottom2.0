#ifndef CELL_H_DEFINED
#define CELL_H_DEFINED

#include "Unit.h"
#include "RandomGenerator.h"
#include "GameConstants.h"
#include <utility>
#include <cmath>
#include <vector>
#include <QPointF>

class Hex
{
private:
    int q,r;
    bool IsVisible;
    bool IsExplored;
    Unit* Unit_;

public:
    Hex(int q_, int r_) : q(q_), r(r_), IsVisible(false), IsExplored(false), Unit_(nullptr)
    {}

    std::pair<int, int> GetQR() const;
    bool VisibilityState() const;
    bool ExplorationState() const;
    QPointF GetCenter() const;
    std::vector<QPointF> GetCorners() const;
    bool IsNeighbor(const Hex& OHex) const;
    friend class HexMap;
    void SetUnit(Unit* UnitTemp);
    Unit* GetUnit() const;
    bool HaveUnit() const;
    void ClearUnit();
};

#endif //CELL_H_DEFINED
