#ifndef UNITFABRIC_H_INCLUDE
#define UNITFABRIC_H_INCLUDE
#include "Unit.h"
#include <memory>
#include <vector>
#include <QPoint>

class UnitFabric
{
private:
    std::vector<std::unique_ptr<Unit>> Units;

public:
    UnitFabric();
    ~UnitFabric();

    Unit* Create(UnitType type, double level, QPoint pos = QPoint(0, 0));
    Unit* Get(int pos);
    int HowMany();
    void Remove(int pos);
    void ClearAll();
};

#endif //UNITFABRIC_H_INCLUDE
