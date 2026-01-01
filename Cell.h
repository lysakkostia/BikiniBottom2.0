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
    bool isVisible;
    bool isExplored;
    Unit* unit;

public:
    Hex(int q_, int r_) : q(q_), r(r_), isVisible(false), isExplored(false), unit(nullptr)
    {}

    friend class HexMap;

    std::pair<int, int> getQR() const;
    QPointF getCenter() const;
    std::vector<QPointF> getCorners() const;
    bool isNeighbor(const Hex& OHex) const;

    bool visibilityState() const;
    bool explorationState() const;

    void setUnit(Unit* UnitTemp);
    Unit* getUnit() const;
    bool haveUnit() const;
    void clearUnit();
};

#endif //CELL_H_DEFINED
