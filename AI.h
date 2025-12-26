#ifndef AI_H_INCLUDE
#define AI_H_INCLUDE

#include <iostream>
#include <vector>
#include <string>
#include "SpellManager.h"

class Unit;

class AI
{
public:
    AI();
    virtual ~AI() = default;

    int TurnOver;

    virtual void InitializeSpells(int level, bool isHero, const std::vector<std::string>& heroSpells = {});
    virtual const Spell* ChooseBestSpell(double currentMana) const;
    const std::vector<Spell> &GetSpells() const;
    void ApplyMultipliers(const QMap<SpellType, double>& multipliers);
    void ApplyManaReductions(const QMap<SpellType, double>& reductions);

protected:
    std::vector<Spell> Spells;
};

class Aggresive : public AI
{
public:
    Aggresive();
};

class Confused : public AI
{
public:
    Confused();
    const Spell* ChooseBestSpell(double currentMana) const override;
};

class Intelligent : public AI
{
public:
    Intelligent();
    void UpgradeSpells();
};

class MainCharacter : public AI
{
public:
    MainCharacter();
    void updateSpellStats(int playerLevel, const std::vector<std::string>& heroSpells);
};

class Friendly : public AI
{
public:
    Friendly();
    std::string getGreeting() const;
};

class Campfire: public AI{
    public:
    Campfire();
    void Heal(Unit* target);
};

#endif
