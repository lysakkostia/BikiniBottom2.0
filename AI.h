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

    virtual void InitializeSpells(int level, bool isHero);
    virtual const Spell* ChooseBestSpell(double currentMana) const;
    const std::vector<Spell> &GetSpells() const;

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
    void updateSpellStats(int playerLevel);
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
