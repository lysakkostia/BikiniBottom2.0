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
    double UHp;
    double UMaxHp;
    double UMana;
    double UMaxMana;
    double ULevel;
    UnitType UType;

protected:
    QPoint UPosition;
    std::unique_ptr<AI> UAi;

public:
    Unit(UnitType type, double level, double hp, double mana, QPoint pos = QPoint(-1, -1));
    virtual ~Unit() = default;

    UnitType GetType() const { return UType; };
    double GetHP() const { return UHp; };
    double GetMaxHP() const { return UMaxHp; };
    double GetMana() const { return UMana; };
    double GetMaxMana() const { return UMaxMana; };
    double GetLevel() const { return ULevel; };
    QPoint GetPosition() const { return UPosition; }
    AI* GetAI() const { return UAi.get(); };

    bool IsEnemy() const;
    bool IsStructure() const;
    bool IsInteractive() const;
    std::string GetTypeString() const;

    void SetPosition(const QPoint& pos);
    void SetHP(double hp);
    void SetMana(double mana);
    void SetLevel(double level);
    void SetMaxHP(double maxHp);
    void SetMaxMana(double maxMana);
    virtual void RecalculateStats() {};
    void SetAI(std::unique_ptr<AI> newAI);

    void TakeDamage(double damage);
    bool CanUseMana(double cost) const;
    void ConsumeMana(double cost);
    virtual void LevelUp();

    virtual QJsonObject ToJson() const;
    virtual void FromJson(const QJsonObject& json);
};

class MainHero : public Unit
{
private:
    double UCurrentXP;
    double UMaxXP;
    int USkillPoints;

    int PendingLevelUps;

    double UBonusMaxHP;
    double UBonusMaxMana;
    QMap<SpellType, double> USpellDamageMultipliers;

    std::vector<std::string> UnlockedSpellIds;
    QSet<QString> UnlockedSkillNodes;
    QMap<SpellType, double> ManaCostReductions;
    void RefreshAISpells();

public:
    MainHero(QPoint Pos);
    virtual void LevelUp() override;
    virtual void RecalculateStats() override;

    double GetCurrentXP() const { return UCurrentXP; }
    double GetMaxXP() const { return UMaxXP; }
    int GetSkillPoints() const { return USkillPoints; }

    void AddXP(double amount);
    void AddSkillPoints();

    void AddMaxHPBonus(double amount);
    void AddMaxManaBonus(double amount);
    void AddSpellDamageMultiplier(SpellType type, double multiplier);
    double GetSpellDamageMultiplier(SpellType type) const;

    bool IsLevelUpPending() const { return PendingLevelUps > 0; }
    void DecrementLevelUpPending() { if(PendingLevelUps > 0) PendingLevelUps--; }
    int GetPendingLevelUpsCount() const { return PendingLevelUps; }

    void ApplyUpgrade(const UpgradeOption& option);

    bool UnlockSkillNode(const std::string& nodeId, int cost);
    bool IsNodeUnlocked(const std::string& nodeId) const;
    void UnlockSpell(const std::string& spellId);

    double GetManaCostMultiplier(SpellType type) const;
    void AddManaCostReduction(SpellType type, double reductionPercent);

    const std::vector<std::string>& GetUnlockedSpellIds() const { return UnlockedSpellIds; }

    virtual QJsonObject ToJson() const override;
    virtual void FromJson(const QJsonObject& json) override;
};

class Enemy : public Unit
{
public:
    Enemy(UnitType type, double level);
    virtual void RecalculateStats() override;
};

class Friend : public Unit
{
public:
    Friend();
    virtual void RecalculateStats() override;
};

class StructUnBreak : public Unit
{
public:
    StructUnBreak();
    virtual void RecalculateStats() override;
};

class StructBreak : public Unit
{
public:
    StructBreak();
    virtual void RecalculateStats() override;
};

class CampfireUnit : public Unit
{
public:
    CampfireUnit();
    virtual void RecalculateStats() override;
    Campfire* GetCampfireAI() const { return dynamic_cast<Campfire*>(UAi.get()); }
};

class Wizard : public Enemy
{
public:
    Wizard(double level);
    virtual void RecalculateStats() override;
};

class Barbarian : public Enemy
{
public:
    Barbarian(double level);
    virtual void RecalculateStats() override;
};

class Warrior : public Enemy
{
public:
    Warrior(double level);
    virtual void RecalculateStats() override;
};

#endif // UNIT_H_INCLUDE
