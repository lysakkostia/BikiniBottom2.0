#include "UnitFabric.h"
#include "AI.h"
#include <iostream>

UnitFabric::UnitFabric() {}

UnitFabric::~UnitFabric()
{
    units.clear();
}

Unit* UnitFabric::create(UnitType type, double level, QPoint pos)
{
    std::unique_ptr<Unit> newUnit = nullptr;

    switch(type)
    {
    case UnitType::MainHero:
        newUnit = std::make_unique<MainHero>(pos);
        if(level > 1)
        {
            newUnit->setLevel(level);
            newUnit->recalculateStats();
            newUnit->setHP(newUnit->getMaxHP());
            newUnit->setMana(newUnit->getMaxMana());
        }
        break;

    case UnitType::Wizard:
        newUnit = std::make_unique<Wizard>(level);
        break;

    case UnitType::Barbarian:
        newUnit = std::make_unique<Barbarian>(level);
        break;

    case UnitType::Warrior:
        newUnit = std::make_unique<Warrior>(level);
        break;

    case UnitType::Friend:
        newUnit = std::make_unique<Friend>();
        break;

    case UnitType::StructBreak:
        newUnit = std::make_unique<StructBreak>();
        break;

    case UnitType::StructUnBreak:
        newUnit = std::make_unique<StructUnBreak>();
        break;

    case UnitType::CampfireUnit:
        newUnit = std::make_unique<CampfireUnit>();
        break;

    default:
        std::cerr << "Unknown UnitType!" << std::endl;
        return nullptr;
    }

    if (!newUnit) {
        return nullptr;
    }

    newUnit->setPosition(pos);
    units.push_back(std::move(newUnit));
    return units.back().get();
}

Unit *UnitFabric::get(int pos)
{
    if (pos < 0 || pos >= units.size()) {
        std::cerr << "Out of bounds: " << pos << std::endl;
        return NULL;
    }
    return units[pos].get();
}

void UnitFabric::remove(int pos)
{
    if (pos < 0 || pos >= units.size()) {
        std::cerr << "Out of bounds: " << pos << std::endl;
        return;
    }
    units.erase(units.begin() + pos);
}

void UnitFabric::clearAll()
{
    units.clear();
}

int UnitFabric::howMany()
{
    return units.size();
}
