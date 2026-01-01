#include <qdebug.h>
#include <algorithm>
#include "Unit.h"
#include "AI.h"
#include "GameConstants.h"

const double HEALTH_EPSILON = 1e-9;

Unit::Unit(UnitType type_, double level_, double hp_, double mana_, QPoint pos_)
{
    type = type_;
    maxHp = (hp_ > 0) ? hp_ : 100;
    maxMana = (mana_ > 0) ? mana_ : 100;
    setHP(hp_);
    setMana(mana_);
    setLevel(level_);
    setPosition(pos_);
    ai = nullptr;
}

bool Unit::isEnemy() const
{
    return type == UnitType::Enemy ||
           type == UnitType::Wizard ||
           type == UnitType::Barbarian ||
           type == UnitType::Warrior;
}

bool Unit::isStructure() const
{
    return type == UnitType::StructBreak ||
           type == UnitType::StructUnBreak ||
           type == UnitType::CampfireUnit;
}

bool Unit::isInteractive() const
{
    return type == UnitType::CampfireUnit ||
           type == UnitType::Friend;
}

std::string Unit::getTypeString() const {
    switch (type) {
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

void Unit::setPosition(const QPoint& pos_)
{
    position = pos_;
}

void Unit::setHP(double hp_)
{
    hp = hp_;

    if (hp > maxHp) hp = maxHp;
    if (hp < 0) hp = 0;
}

void Unit::setMaxHP(double maxHp_)
{
    if (maxHp_ < 1) maxHp_ = 1;
    maxHp = maxHp_;
    if (hp > maxHp) hp = maxHp;
}

void Unit::setMana(double mana_)
{
    mana = mana_;

    if (mana > maxMana) mana = maxMana;
    if (mana < 0) mana = 0;
}

void Unit::setMaxMana(double maxMana_)
{
    if (maxMana_ < 0) maxMana_ = 0;
    maxMana = maxMana_;
    if (mana > maxMana) mana = maxMana;
}

void Unit::setLevel(double level_)
{
    level = level_;
}

void Unit::setAI(std::unique_ptr<AI> newAI)
{
    ai = std::move(newAI);
}

void Unit::takeDamage(double damage) {
    hp -= damage;
    if (hp < HEALTH_EPSILON) hp = 0;
}

bool Unit::canUseMana(double cost) const {
    return mana >= cost;
}

void Unit::consumeMana(double cost) {
    if (canUseMana(cost)) {
        mana -= cost;
    }
}

void Unit::levelUp()
{
    setLevel(level + 1);
    recalculateStats();
    hp = maxHp;
    mana = maxMana;
}

QJsonObject Unit::toJson() const
{
    QJsonObject json;

    json["type"] = static_cast<int>(type);
    json["hp"] = hp;
    json["mana"] = mana;
    json["level"] = level;
    json["x"] = position.x();
    json["y"] = position.y();

    return json;
}

void Unit::fromJson(const QJsonObject& json)
{
    if (json.contains("type")) type = static_cast<UnitType>(json["type"].toInt());
    if (json.contains("level")) level = json["level"].toDouble();

    recalculateStats();

    if (json.contains("hp")) setHP(json["hp"].toDouble());
    if (json.contains("mana")) setMana(json["mana"].toDouble());

    if (json.contains("x") && json.contains("y")) {
        position = QPoint(json["x"].toInt(), json["y"].toInt());
    }
}

//----MainHero----

MainHero::MainHero(QPoint Pos)
    : Unit(UnitType::MainHero, 1, 0, 0, Pos)
{
    currentXP = 0;
    maxXP = GlobalConst::Progression::BASE_XP_REQ;
    skillPoints = 0;
    bonusMaxHP = 0;
    bonusMaxMana = 0;
    pendingLevelUps = 0;

    setAI(std::make_unique<MainCharacter>());

    unlockedSpellIds.push_back("ember");
    unlockedSpellIds.push_back("icicle");
    unlockedSpellIds.push_back("spark");
    unlockedSpellIds.push_back("poison_dart");
    unlockedSpellIds.push_back("dark_orb");
    unlockedSpellIds.push_back("energy_pulse");

    if (ai) {
        MainCharacter* heroAI = dynamic_cast<MainCharacter*>(ai.get());
        if(heroAI) heroAI->updateSpellStats(1, unlockedSpellIds);
    }

    recalculateStats();
    setHP(getMaxHP());
    setMana(getMaxMana());
}

void MainHero::addXP(double amount)
{
    currentXP += amount;
    while(currentXP >= maxXP)
    {
        levelUp();
    }
}

void MainHero::levelUp()
{
    currentXP -= maxXP;
    setLevel(getLevel() + 1);

    maxXP = maxXP * GlobalConst::Progression::XP_GROWTH_FACTOR;
    addSkillPoints();
    pendingLevelUps++;

    refreshAISpells();

    recalculateStats();
    setHP(getMaxHP());
    setMana(getMaxMana());
}

void MainHero::addSkillPoints()
{
    int currentLevelInt = static_cast<int>(getLevel());
    int pointsToGain = 1;

    if (currentLevelInt % 5 == 0) {
        pointsToGain = 2;
        qDebug() << "Bonus Level! Gained 2 Skill Points.";
    }
    skillPoints += pointsToGain;
}

void MainHero::addMaxHPBonus(double amount) {
    bonusMaxHP += amount;
    recalculateStats();
    setHP(getHP() + amount);
}

void MainHero::addMaxManaBonus(double amount) {
    bonusMaxMana += amount;
    recalculateStats();
    setMana(getMana() + amount);
}

void MainHero::addSpellDamageMultiplier(SpellType type, double multiplier) {
    if(!spellDamageMultipliers.contains(type)) {
        spellDamageMultipliers[type] = 1.0;
    }
    spellDamageMultipliers[type] += multiplier;
    refreshAISpells();
}

double MainHero::getSpellDamageMultiplier(SpellType type) const {
    return spellDamageMultipliers.value(type, 1.0);
}

void MainHero::recalculateStats()
{
    double baseMaxHp = GlobalConst::GLOBAL_BASE_HP + (getLevel() * GlobalConst::Hero::HP_GROWTH);
    double baseMaxMana = GlobalConst::GLOBAL_BASE_MANA + (getLevel() * GlobalConst::Hero::MANA_GROWTH);

    setMaxHP(baseMaxHp + bonusMaxHP);
    setMaxMana(baseMaxMana + bonusMaxMana);
}

void MainHero::applyUpgrade(const UpgradeOption& option)
{
    if (option.type == UpgradeType::StatIncrease) {
        if (option.statIndex == 0) {
            addMaxHPBonus(option.value);
            setHP(getHP() + option.value);
        } else if (option.statIndex == 1) {
            addMaxManaBonus(option.value);
            setMana(getMana() + option.value);
        }
    }
    else if (option.type == UpgradeType::SpellTypeBuff) {
        addSpellDamageMultiplier(option.specificType, option.value);
        qDebug() << "Buffed" << (int)option.specificType << "by" << option.value;
    }
}

bool MainHero::unlockSkillNode(const std::string& nodeId, int cost) {
    if (skillPoints >= cost) {
        skillPoints -= cost;
        unlockedSkillNodes.insert(QString::fromStdString(nodeId));
        return true;
    }
    return false;
}

bool MainHero::isNodeUnlocked(const std::string& nodeId) const {
    return unlockedSkillNodes.contains(QString::fromStdString(nodeId));
}

void MainHero::unlockSpell(const std::string& spellId) {
    for(const auto& id : unlockedSpellIds) {
        if(id == spellId) return;
    }
    unlockedSpellIds.push_back(spellId);

    refreshAISpells();
}

void MainHero::addManaCostReduction(SpellType type, double reductionPercent) {
    if (!manaCostReductions.contains(type)) manaCostReductions[type] = 0.0;
    manaCostReductions[type] += reductionPercent;
    refreshAISpells();
}

double MainHero::getManaCostMultiplier(SpellType type) const {
    double reduction = manaCostReductions.value(type, 0.0);
    double mult = 1.0 - reduction;
    if (mult < 0.1) mult = 0.1;
    return mult;
}

void MainHero::refreshAISpells()
{
    if (!ai) return;

    MainCharacter* heroAI = dynamic_cast<MainCharacter*>(ai.get());
    if (heroAI) {
        heroAI->updateSpellStats(this->getLevel(), unlockedSpellIds);
        ai->applyMultipliers(spellDamageMultipliers);
        ai->applyManaReductions(manaCostReductions);
    }
}

QJsonObject MainHero::toJson() const
{
    QJsonObject json = Unit::toJson();

    json["xp"] = currentXP;
    json["skillPoints"] = skillPoints;
    json["pendingLevelUps"] = pendingLevelUps;
    json["bonusHp"] = bonusMaxHP;
    json["bonusMana"] = bonusMaxMana;

    QJsonArray spellsArr;
    for(const auto& s : unlockedSpellIds) {
        spellsArr.append(QString::fromStdString(s));
    }
    json["unlockedSpells"] = spellsArr;

    QJsonArray nodesArr;
    for(const auto& node : unlockedSkillNodes) {
        nodesArr.append(node);
    }
    json["unlockedNodes"] = nodesArr;

    QJsonObject dmgMultObj;
    for(auto it = spellDamageMultipliers.begin(); it != spellDamageMultipliers.end(); ++it) {
        dmgMultObj[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    json["dmgMultipliers"] = dmgMultObj;

    QJsonObject manaRedObj;
    for(auto it = manaCostReductions.begin(); it != manaCostReductions.end(); ++it) {
        manaRedObj[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    json["manaReductions"] = manaRedObj;

    return json;
}

void MainHero::fromJson(const QJsonObject& json)
{
    Unit::fromJson(json);

    if (json.contains("xp")) currentXP = json["xp"].toDouble();
    if (json.contains("skillPoints")) skillPoints = json["skillPoints"].toInt();
    if (json.contains("pendingLevelUps")) pendingLevelUps = json["pendingLevelUps"].toInt();
    if (json.contains("bonusHp")) bonusMaxHP = json["bonusHp"].toDouble();
    if (json.contains("bonusMana")) bonusMaxMana = json["bonusMana"].toDouble();

    if (json.contains("unlockedSpells")) {
        unlockedSpellIds.clear();
        QJsonArray spellsArr = json["unlockedSpells"].toArray();
        for(const auto& val : spellsArr) {
            unlockedSpellIds.push_back(val.toString().toStdString());
        }
    }

    if (json.contains("unlockedNodes")) {
        unlockedSkillNodes.clear();
        QJsonArray nodesArr = json["unlockedNodes"].toArray();
        for(const auto& val : nodesArr) {
            unlockedSkillNodes.insert(val.toString());
        }
    }

    if (json.contains("dmgMultipliers")) {
        spellDamageMultipliers.clear();
        QJsonObject obj = json["dmgMultipliers"].toObject();
        for(auto it = obj.begin(); it != obj.end(); ++it) {
            spellDamageMultipliers[static_cast<SpellType>(it.key().toInt())] = it.value().toDouble();
        }
    }

    if (json.contains("manaReductions")) {
        manaCostReductions.clear();
        QJsonObject obj = json["manaReductions"].toObject();
        for(auto it = obj.begin(); it != obj.end(); ++it) {
            manaCostReductions[static_cast<SpellType>(it.key().toInt())] = it.value().toDouble();
        }
    }

    recalculateStats();
    refreshAISpells();

    if (json.contains("hp")) setHP(json["hp"].toDouble());
    if (json.contains("mana")) setMana(json["mana"].toDouble());
}

//----Enemy----

Enemy::Enemy(UnitType type, double level)
    : Unit(type, level, 0, 0)
{
    setAI(std::make_unique<Aggresive>());
    if (ai) {
        ai->initializeSpells(static_cast<int>(level), false);
    }
}

void Enemy::recalculateStats()
{
    //real values in child classes
    setMaxHP(100);
    setMaxMana(0);
}

Wizard::Wizard(double level)
    : Enemy(UnitType::Wizard, level)
{
    setAI(std::make_unique<Intelligent>());

    recalculateStats();
    setHP(getMaxHP());
    setMana(getMaxMana());

    if (ai) {
        ai->initializeSpells(static_cast<int>(level), false);
        static_cast<Intelligent*>(ai.get())->upgradeSpells();
    }
}

void Wizard::recalculateStats()
{
    double level = getLevel();
    double finalHp = (GlobalConst::Wizard::HP_MULT + level / GlobalConst::Wizard::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Wizard::MANA_MULT + level / GlobalConst::Wizard::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    setMaxHP(finalHp);
    setMaxMana(finalMana);
}

Barbarian::Barbarian(double level)
    : Enemy(UnitType::Barbarian, level)
{
    setAI(std::make_unique<Aggresive>());

    recalculateStats();
    setHP(getMaxHP());
    setMana(getMaxMana());

    if (ai) {
        ai->initializeSpells(static_cast<int>(level), false);
    }
}

void Barbarian::recalculateStats()
{
    double level = getLevel();
    double finalHp = (GlobalConst::Barbarian::HP_MULT + level / GlobalConst::Barbarian::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Barbarian::MANA_MULT + level / GlobalConst::Barbarian::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    setMaxHP(finalHp);
    setMaxMana(finalMana);
}

Warrior::Warrior(double level)
    : Enemy(UnitType::Warrior, level)
{
    setAI(std::make_unique<Confused>());

    recalculateStats();
    setHP(getMaxHP());
    setMana(getMaxMana());

    if (ai) {
        ai->initializeSpells(static_cast<int>(level), false);
    }
}

void Warrior::recalculateStats()
{
    double level = getLevel();
    double finalHp = (GlobalConst::Warrior::HP_MULT + level / GlobalConst::Warrior::HP_DIV) * GlobalConst::GLOBAL_BASE_HP;
    double finalMana = (GlobalConst::Warrior::MANA_MULT + level / GlobalConst::Warrior::MANA_DIV) * GlobalConst::GLOBAL_BASE_MANA;

    setMaxHP(finalHp);
    setMaxMana(finalMana);
}

Friend::Friend() : Unit(UnitType::Friend, 1, 100, 0)
{
    setAI(std::make_unique<Friendly>());
    recalculateStats();
}
void Friend::recalculateStats()
{
    setMaxHP(100);
    setMaxMana(0);
}

StructBreak::StructBreak() : Unit(UnitType::StructBreak, 1, 0, 0)
{
    setAI(nullptr);
    recalculateStats();
    setHP(getMaxHP());
}
void StructBreak::recalculateStats()
{
    setMaxHP(GlobalConst::Structures::BREAKABLE_HP);
    setMaxMana(0);
}

StructUnBreak::StructUnBreak() : Unit(UnitType::StructUnBreak, 1, 0, 0)
{
    setAI(nullptr);
    recalculateStats();
}
void StructUnBreak::recalculateStats()
{
    setMaxHP(99999);
    setMaxMana(0);
}


CampfireUnit::CampfireUnit() : Unit(UnitType::CampfireUnit, 1, 0, 0)
{
    setAI(std::make_unique<Campfire>());
    recalculateStats();
    setHP(getMaxHP());
}
void CampfireUnit::recalculateStats()
{
    setMaxHP(GlobalConst::Structures::CAMPFIRE_HP);
    setMaxMana(0);
}
