#ifndef UNIT_H_INCLUDE
#define UNIT_H_INCLUDE

#include "AI.h"
#include "SpellManager.h"
#include "LevelUpSystem.h"
#include <string>
#include <utility>
#include <QPoint>
#include <QJsonObject>
#include <iostream>
#include <QMap>
#include <QSet>

enum class UnitType
{
    MainHero,
    Enemy,
    Friend,
    StructUnBreak,
    StructBreak,
    CampfireUnit,
    Wizard,
    Barbarian,
    Warrior
};

class Unit
{
private:
    double hp;
    double maxHp;
    double mana;
    double maxMana;
    double level;
    UnitType type;

protected:
    QPoint position;
    std::unique_ptr<AI> ai;

public:
    Unit(UnitType type_, double level_, double hp_, double mana_, QPoint pos_ = QPoint(-1, -1));
    virtual ~Unit() = default;

    UnitType getType() const { return type; };
    double getHP() const { return hp; };
    double getMaxHP() const { return maxHp; };
    double getMana() const { return mana; };
    double getMaxMana() const { return maxMana; };
    double getLevel() const { return level; };
    QPoint getPosition() const { return position; }
    AI* getAI() const { return ai.get(); };

    bool isEnemy() const;
    bool isStructure() const;
    bool isInteractive() const;
    std::string getTypeString() const;

    void setPosition(const QPoint& pos_);
    void setHP(double hp_);
    void setMana(double mana_);
    void setLevel(double level_);
    void setMaxHP(double maxHp_);
    void setMaxMana(double maxMana_);
    virtual void recalculateStats() {};
    void setAI(std::unique_ptr<AI> newAI);

    void takeDamage(double damage);
    bool canUseMana(double cost) const;
    void consumeMana(double cost);
    virtual void levelUp();
    virtual void applyUpgrade(const UpgradeOption& option) {};

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);
};

class MainHero : public Unit
{
private:
    double currentXP;
    double maxXP;
    int skillPoints;

    int pendingLevelUps;

    double bonusMaxHP;
    double bonusMaxMana;
    QMap<SpellType, double> spellDamageMultipliers;

    std::vector<std::string> unlockedSpellIds;
    QSet<QString> unlockedSkillNodes;
    QMap<SpellType, double> manaCostReductions;
    void refreshAISpells();

public:
    MainHero(QPoint Pos);
    virtual void levelUp() override;
    virtual void recalculateStats() override;

    double getCurrentXP() const { return currentXP; }
    double getMaxXP() const { return maxXP; }
    int getSkillPoints() const { return skillPoints; }

    void addXP(double amount);
    void addSkillPoints();

    void addMaxHPBonus(double amount);
    void addMaxManaBonus(double amount);
    void addSpellDamageMultiplier(SpellType type, double multiplier);
    double getSpellDamageMultiplier(SpellType type) const;

    bool isLevelUpPending() const { return pendingLevelUps > 0; }
    void decrementLevelUpPending() { if(pendingLevelUps > 0) pendingLevelUps--; }
    int getPendingLevelUpsCount() const { return pendingLevelUps; }

    virtual void applyUpgrade(const UpgradeOption& option) override;

    bool unlockSkillNode(const std::string& nodeId, int cost);
    bool isNodeUnlocked(const std::string& nodeId) const;
    void unlockSpell(const std::string& spellId);

    double getManaCostMultiplier(SpellType type) const;
    void addManaCostReduction(SpellType type, double reductionPercent);

    const std::vector<std::string>& getUnlockedSpellIds() const { return unlockedSpellIds; }

    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;
};

class Enemy : public Unit
{
private:
    QMap<SpellType, double> dmgMultipliers;

public:
    Enemy(UnitType type, double level);
    virtual void recalculateStats() override;
    virtual void applyUpgrade(const UpgradeOption& option) override;
};

class Friend : public Unit
{
public:
    Friend();
    virtual void recalculateStats() override;
};

class StructUnBreak : public Unit
{
public:
    StructUnBreak();
    virtual void recalculateStats() override;
};

class StructBreak : public Unit
{
public:
    StructBreak();
    virtual void recalculateStats() override;
};

class CampfireUnit : public Unit
{
public:
    CampfireUnit();
    virtual void recalculateStats() override;
    Campfire* getCampfireAI() const { return dynamic_cast<Campfire*>(ai.get()); }
};

class Wizard : public Enemy
{
public:
    Wizard(double level);
    virtual void recalculateStats() override;
};

class Barbarian : public Enemy
{
public:
    Barbarian(double level);
    virtual void recalculateStats() override;
};

class Warrior : public Enemy
{
public:
    Warrior(double level);
    virtual void recalculateStats() override;
};

#endif // UNIT_H_INCLUDE
