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

//----MainHero----

MainHero::MainHero(QPoint Pos)
    : Unit(UnitType::MainHero, 1, 0, 0, Pos)
{
    UCurrentXP = 0;
    UMaxXP = GlobalConst::Progression::BASE_XP_REQ;
    USkillPoints = 0;
    UBonusMaxHP = 0;
    UBonusMaxMana = 0;
    PendingLevelUps = 0;

    SetAI(std::make_unique<MainCharacter>());

    UnlockedSpellIds.push_back("ember");
    UnlockedSpellIds.push_back("icicle");
    UnlockedSpellIds.push_back("spark");
    UnlockedSpellIds.push_back("poison_dart");
    UnlockedSpellIds.push_back("dark_orb");
    UnlockedSpellIds.push_back("energy_pulse");

    if (UAi) {
        MainCharacter* heroAI = dynamic_cast<MainCharacter*>(UAi.get());
        if(heroAI) heroAI->updateSpellStats(1, UnlockedSpellIds);
    }

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void MainHero::AddXP(double amount)
{
    UCurrentXP += amount;
    while(UCurrentXP >= UMaxXP)
    {
        LevelUp();
    }
}

void MainHero::LevelUp()
{
    UCurrentXP -= UMaxXP;
    SetLevel(GetLevel() + 1);

    UMaxXP = UMaxXP * GlobalConst::Progression::XP_GROWTH_FACTOR;
    AddSkillPoints();
    PendingLevelUps++;

    RefreshAISpells();

    RecalculateStats();
    SetHP(GetMaxHP());
    SetMana(GetMaxMana());
}

void MainHero::AddSkillPoints()
{
    int currentLevelInt = static_cast<int>(GetLevel());
    int pointsToGain = 1;

    if (currentLevelInt % 5 == 0) {
        pointsToGain = 2;
        qDebug() << "Bonus Level! Gained 2 Skill Points.";
    }
    USkillPoints += pointsToGain;
}

void MainHero::AddMaxHPBonus(double amount) {
    UBonusMaxHP += amount;
    RecalculateStats();
    SetHP(GetHP() + amount);
}

void MainHero::AddMaxManaBonus(double amount) {
    UBonusMaxMana += amount;
    RecalculateStats();
    SetMana(GetMana() + amount);
}

void MainHero::AddSpellDamageMultiplier(SpellType type, double multiplier) {
    if(!USpellDamageMultipliers.contains(type)) {
        USpellDamageMultipliers[type] = 1.0;
    }
    USpellDamageMultipliers[type] += multiplier;
    RefreshAISpells();
}

double MainHero::GetSpellDamageMultiplier(SpellType type) const {
    return USpellDamageMultipliers.value(type, 1.0);
}

void MainHero::RecalculateStats()
{
    double baseMaxHp = GlobalConst::GLOBAL_BASE_HP + (GetLevel() * GlobalConst::Hero::HP_GROWTH);
    double baseMaxMana = GlobalConst::GLOBAL_BASE_MANA + (GetLevel() * GlobalConst::Hero::MANA_GROWTH);

    SetMaxHP(baseMaxHp + UBonusMaxHP);
    SetMaxMana(baseMaxMana + UBonusMaxMana);
}

void MainHero::ApplyUpgrade(const UpgradeOption& option)
{
    if (option.type == UpgradeType::StatIncrease) {
        if (option.statIndex == 0) {
            AddMaxHPBonus(option.value);
            SetHP(GetHP() + option.value);
        } else if (option.statIndex == 1) {
            AddMaxManaBonus(option.value);
            SetMana(GetMana() + option.value);
        }
    }
    else if (option.type == UpgradeType::SpellTypeBuff) {
        AddSpellDamageMultiplier(option.specificType, option.value);
        qDebug() << "Buffed" << (int)option.specificType << "by" << option.value;
    }
}

bool MainHero::UnlockSkillNode(const std::string& nodeId, int cost) {
    if (USkillPoints >= cost) {
        USkillPoints -= cost;
        UnlockedSkillNodes.insert(QString::fromStdString(nodeId));
        return true;
    }
    return false;
}

bool MainHero::IsNodeUnlocked(const std::string& nodeId) const {
    return UnlockedSkillNodes.contains(QString::fromStdString(nodeId));
}

void MainHero::UnlockSpell(const std::string& spellId) {
    for(const auto& id : UnlockedSpellIds) {
        if(id == spellId) return;
    }
    UnlockedSpellIds.push_back(spellId);

    RefreshAISpells();
}

void MainHero::AddManaCostReduction(SpellType type, double reductionPercent) {
    if (!ManaCostReductions.contains(type)) ManaCostReductions[type] = 0.0;
    ManaCostReductions[type] += reductionPercent;
    RefreshAISpells();
}

double MainHero::GetManaCostMultiplier(SpellType type) const {
    double reduction = ManaCostReductions.value(type, 0.0);
    double mult = 1.0 - reduction;
    if (mult < 0.1) mult = 0.1;
    return mult;
}

void MainHero::RefreshAISpells()
{
    if (!UAi) return;

    MainCharacter* heroAI = dynamic_cast<MainCharacter*>(UAi.get());
    if (heroAI) {
        heroAI->updateSpellStats(this->GetLevel(), UnlockedSpellIds);
        UAi->ApplyMultipliers(USpellDamageMultipliers);
        UAi->ApplyManaReductions(ManaCostReductions);
    }
}

QJsonObject MainHero::ToJson() const
{
    QJsonObject json = Unit::ToJson();

    json["xp"] = UCurrentXP;
    json["skillPoints"] = USkillPoints;
    json["pendingLevelUps"] = PendingLevelUps;
    json["bonusHp"] = UBonusMaxHP;
    json["bonusMana"] = UBonusMaxMana;

    QJsonArray spellsArr;
    for(const auto& s : UnlockedSpellIds) {
        spellsArr.append(QString::fromStdString(s));
    }
    json["unlockedSpells"] = spellsArr;

    QJsonArray nodesArr;
    for(const auto& node : UnlockedSkillNodes) {
        nodesArr.append(node);
    }
    json["unlockedNodes"] = nodesArr;

    QJsonObject dmgMultObj;
    for(auto it = USpellDamageMultipliers.begin(); it != USpellDamageMultipliers.end(); ++it) {
        dmgMultObj[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    json["dmgMultipliers"] = dmgMultObj;

    QJsonObject manaRedObj;
    for(auto it = ManaCostReductions.begin(); it != ManaCostReductions.end(); ++it) {
        manaRedObj[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    json["manaReductions"] = manaRedObj;

    return json;
}

void MainHero::FromJson(const QJsonObject& json)
{
    Unit::FromJson(json);

    if (json.contains("xp")) UCurrentXP = json["xp"].toDouble();
    if (json.contains("skillPoints")) USkillPoints = json["skillPoints"].toInt();
    if (json.contains("pendingLevelUps")) PendingLevelUps = json["pendingLevelUps"].toInt();
    if (json.contains("bonusHp")) UBonusMaxHP = json["bonusHp"].toDouble();
    if (json.contains("bonusMana")) UBonusMaxMana = json["bonusMana"].toDouble();

    if (json.contains("unlockedSpells")) {
        UnlockedSpellIds.clear();
        QJsonArray spellsArr = json["unlockedSpells"].toArray();
        for(const auto& val : spellsArr) {
            UnlockedSpellIds.push_back(val.toString().toStdString());
        }
    }

    if (json.contains("unlockedNodes")) {
        UnlockedSkillNodes.clear();
        QJsonArray nodesArr = json["unlockedNodes"].toArray();
        for(const auto& val : nodesArr) {
            UnlockedSkillNodes.insert(val.toString());
        }
    }

    if (json.contains("dmgMultipliers")) {
        USpellDamageMultipliers.clear();
        QJsonObject obj = json["dmgMultipliers"].toObject();
        for(auto it = obj.begin(); it != obj.end(); ++it) {
            USpellDamageMultipliers[static_cast<SpellType>(it.key().toInt())] = it.value().toDouble();
        }
    }

    if (json.contains("manaReductions")) {
        ManaCostReductions.clear();
        QJsonObject obj = json["manaReductions"].toObject();
        for(auto it = obj.begin(); it != obj.end(); ++it) {
            ManaCostReductions[static_cast<SpellType>(it.key().toInt())] = it.value().toDouble();
        }
    }

    RecalculateStats();
    RefreshAISpells();

    if (json.contains("hp")) SetHP(json["hp"].toDouble());
    if (json.contains("mana")) SetMana(json["mana"].toDouble());
}

//----Enemy----

Enemy::Enemy(UnitType type, double level)
    : Unit(type, level, 0, 0)
{
    SetAI(std::make_unique<Aggresive>());
    if (UAi) {
        UAi->InitializeSpells(static_cast<int>(level), false);
    }
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

    if (UAi) {
        UAi->InitializeSpells(static_cast<int>(level), false);
        static_cast<Intelligent*>(UAi.get())->UpgradeSpells();
    }
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

    if (UAi) {
        UAi->InitializeSpells(static_cast<int>(level), false);
    }
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

    if (UAi) {
        UAi->InitializeSpells(static_cast<int>(level), false);
    }
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
