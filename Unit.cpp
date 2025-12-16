#include <qdebug.h>
#include <algorithm>
#include "Unit.h"
#include "AI.h"
#include "GameConstants.h"

const double HEALTH_EPSILON = 1e-9;

Unit::Unit(UnitType type, double level, double hp, double mana, QPoint pos)
{
    UType = type;
    UMaxHp = (hp > 0) ? hp : 100;
    UMaxMana = (mana > 0) ? mana : 100;
    SetHP(hp);
    SetMana(mana);
    SetLevel(level);
    SetPosition(pos);
    UAi = nullptr;
}

bool Unit::IsEnemy() const
{
    return UType == UnitType::Enemy ||
           UType == UnitType::Wizard ||
           UType == UnitType::Barbarian ||
           UType == UnitType::Warrior;
}

bool Unit::IsStructure() const
{
    return UType == UnitType::StructBreak ||
           UType == UnitType::StructUnBreak ||
           UType == UnitType::CampfireUnit;
}

bool Unit::IsInteractive() const
{
    return UType == UnitType::CampfireUnit ||
           UType == UnitType::Friend;
}

std::string Unit::GetTypeString() const {
    switch (UType) {
    case UnitType::MainHero: return "MainHero";
    case UnitType::Enemy: return "Enemy";
    case UnitType::Wizard: return "Wizard";
    case UnitType::Barbarian: return "Barbarian";
    case UnitType::Warrior: return "Warrior";
    case UnitType::Friend: return "Friend";
    case UnitType::CampfireUnit: return "Campfire";
    case UnitType::StructBreak: return "StructBreak";
    case UnitType::StructUnBreak: return "StructUnBreak";
    default: return "Unknown";
    }
}

void Unit::SetPosition(const QPoint& pos)
{
    UPosition = pos;
}

void Unit::SetHP(double hp)
{
    UHp = hp;

    if (UHp > UMaxHp) UHp = UMaxHp;
    if (UHp < 0) UHp = 0;
}

void Unit::SetMaxHP(double maxHp)
{
    if (maxHp < 1) maxHp = 1;
    UMaxHp = maxHp;
    if (UHp > UMaxHp) UHp = UMaxHp;
}

void Unit::SetMana(double mana)
{
    UMana = mana;

    if (UMana > UMaxMana) UMana = UMaxMana;
    if (UMana < 0) UMana = 0;
}

void Unit::SetMaxMana(double maxMana)
{
    if (maxMana < 0) maxMana = 0;
    UMaxMana = maxMana;
    if (UMana > UMaxMana) UMana = UMaxMana;
}

void Unit::SetLevel(double level)
{
    ULevel = level;
}

void Unit::SetAI(std::unique_ptr<AI> newAI)
{
    UAi = std::move(newAI);
}

void Unit::TakeDamage(double damage) {
    UHp -= damage;
    if (UHp < HEALTH_EPSILON) UHp = 0;
}

bool Unit::CanUseMana(double cost) const {
    return UMana >= cost;
}

void Unit::ConsumeMana(double cost) {
    if (CanUseMana(cost)) {
        UMana -= cost;
    }
}

void Unit::LevelUp()
{
    SetLevel(ULevel + 1);
    RecalculateStats();
    UHp = UMaxHp;
    UMana = UMaxMana;
}

QJsonObject Unit::ToJson() const
{
    QJsonObject json;

    json["type"] = static_cast<int>(UType);
    json["hp"] = UHp;
    json["mana"] = UMana;
    json["level"] = ULevel;
    json["x"] = UPosition.x();
    json["y"] = UPosition.y();

    return json;
}

void Unit::FromJson(const QJsonObject& json)
{
    if (json.contains("type")) UType = static_cast<UnitType>(json["type"].toInt());
    if (json.contains("level")) ULevel = json["level"].toDouble();

    RecalculateStats();

    if (json.contains("hp")) SetHP(json["hp"].toDouble());
    if (json.contains("mana")) SetMana(json["mana"].toDouble());

    if (json.contains("x") && json.contains("y")) {
        UPosition = QPoint(json["x"].toInt(), json["y"].toInt());
    }
}

MainHero::MainHero(QPoint Pos)
    : Unit(UnitType::MainHero, 1, 0, 0, Pos)
{
    SetAI(std::make_unique<MainCharacter>());

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void MainHero::RecalculateStats()
{
    double maxHp = GlobalConst::GLOBAL_BASE_HP + (GetLevel() * GlobalConst::Hero::HP_GROWTH);
    double maxMana = GlobalConst::GLOBAL_BASE_MANA + (GetLevel() * GlobalConst::Hero::MANA_GROWTH);

    SetMaxHP(maxHp);
    SetMaxMana(maxMana);
}

void MainHero::LevelUp()
{
    Unit::LevelUp();

    qDebug()<<"Герой підвищив рівень, новий рівень: " << this->GetLevel()
             <<", HP: " << this->GetHP() <<"/" << GetMaxHP()
             <<", MP: " << this->GetMana() <<"/" << GetMaxMana();
    if (UAi) {
        MainCharacter* heroAI = dynamic_cast<MainCharacter*>(UAi.get());
        if (heroAI) {
            heroAI->updateSpellStats(this->GetLevel());
            qDebug() << "Заклинання героя оновлено для рівня " << this->GetLevel();
        }
    }
}

Enemy::Enemy(UnitType type, double level)
    : Unit(type, level, 0, 0)
{
    SetAI(std::make_unique<Aggresive>());
}

void Enemy::RecalculateStats()
{
    //real values in child classes
    SetMaxHP(100);
    SetMaxMana(0);
}

Wizard::Wizard(double level)
    : Enemy(UnitType::Wizard, level)
{
    SetAI(std::make_unique<Intelligent>());

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void Wizard::RecalculateStats()
{
    double level = GetLevel();
    double finalHp = (GlobalConst::Wizard::HP_MULT + level / GlobalConst::Wizard::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Wizard::MANA_MULT + level / GlobalConst::Wizard::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    SetMaxHP(finalHp);
    SetMaxMana(finalMana);
}

Barbarian::Barbarian(double level)
    : Enemy(UnitType::Barbarian, level)
{
    SetAI(std::make_unique<Aggresive>());

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void Barbarian::RecalculateStats()
{
    double level = GetLevel();
    double finalHp = (GlobalConst::Barbarian::HP_MULT + level / GlobalConst::Barbarian::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Barbarian::MANA_MULT + level / GlobalConst::Barbarian::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    SetMaxHP(finalHp);
    SetMaxMana(finalMana);
}

Warrior::Warrior(double level)
    : Enemy(UnitType::Warrior, level)
{
    SetAI(std::make_unique<Confused>());

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void Warrior::RecalculateStats()
{
    double level = GetLevel();
    double finalHp = (GlobalConst::Warrior::HP_MULT + level / GlobalConst::Warrior::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Warrior::MANA_MULT + level / GlobalConst::Warrior::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    SetMaxHP(finalHp);
    SetMaxMana(finalMana);
}

Friend::Friend() : Unit(UnitType::Friend, 1, 100, 0)
{
    SetAI(std::make_unique<Friendly>());
    RecalculateStats();
}
void Friend::RecalculateStats()
{
    SetMaxHP(100);
    SetMaxMana(0);
}

StructBreak::StructBreak() : Unit(UnitType::StructBreak, 1, 0, 0)
{
    SetAI(nullptr);
    RecalculateStats();
    SetHP(GetMaxHP());
}
void StructBreak::RecalculateStats()
{
    SetMaxHP(GlobalConst::Structures::BREAKABLE_HP);
    SetMaxMana(0);
}

StructUnBreak::StructUnBreak() : Unit(UnitType::StructUnBreak, 1, 0, 0)
{
    SetAI(nullptr);
    RecalculateStats();
}
void StructUnBreak::RecalculateStats()
{
    SetMaxHP(99999);
    SetMaxMana(0);
}


CampfireUnit::CampfireUnit() : Unit(UnitType::CampfireUnit, 1, 0, 0)
{
    SetAI(std::make_unique<Campfire>());
    RecalculateStats();
    SetHP(GetMaxHP());
}
void CampfireUnit::RecalculateStats()
{
    SetMaxHP(GlobalConst::Structures::CAMPFIRE_HP);
    SetMaxMana(0);
}
