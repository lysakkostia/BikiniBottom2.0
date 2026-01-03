#ifndef UNITFABRIC_H_INCLUDE
#define UNITFABRIC_H_INCLUDE
#include "Unit.h"
#include <memory>
#include <vector>
#include <QPoint>

class UnitFabric
{
private:
    std::vector<std::unique_ptr<Unit>> units;

public:
    UnitFabric();
    ~UnitFabric();

    Unit* create(UnitType type, double level, QPoint pos = QPoint(0, 0));
    Unit* get(int pos);
    int howMany();
    void remove(int pos);
    void clearAll();
};

#endif //UNITFABRIC_H_INCLUDE
