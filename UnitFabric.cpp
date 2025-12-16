#include "UnitFabric.h"
#include "AI.h"
#include <iostream>

UnitFabric::UnitFabric() {}

UnitFabric::~UnitFabric()
{
    Units.clear();
}

Unit* UnitFabric::Create(UnitType type, double level, QPoint pos)
{
    std::unique_ptr<Unit> newUnit = nullptr;

    switch(type)
    {
    case UnitType::MainHero:
        newUnit = std::make_unique<MainHero>(pos);
        if(level > 1)
        {
            newUnit->SetLevel(level);
            newUnit->RecalculateStats();
            newUnit->SetHP(newUnit->GetMaxHP());
            newUnit->SetMana(newUnit->GetMaxMana());
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

    newUnit->SetPosition(pos);
    Units.push_back(std::move(newUnit));
    return Units.back().get();
}

Unit *UnitFabric::Get(int pos)
{
    if (pos < 0 || pos >= Units.size()) {
        std::cerr << "Out of bounds: " << pos << std::endl;
        return NULL;
    }
    return Units[pos].get();
}

void UnitFabric::Remove(int pos)
{
    if (pos < 0 || pos >= Units.size()) {
        std::cerr << "Out of bounds: " << pos << std::endl;
        return;
    }
    Units.erase(Units.begin() + pos);
}

void UnitFabric::ClearAll()
{
    Units.clear();
}

int UnitFabric::HowMany()
{
    return Units.size();
}
