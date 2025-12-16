#ifndef AI_H_INCLUDE
#define AI_H_INCLUDE

#include <iostream>
#include <vector>
#include <string>

class Unit;

class Spell
{
public:
    Spell(std::string name, double manacost, double damage);
    std::string name;
    double manacost;
    double damage;
};

class AI
{
public:
    AI();
    virtual ~AI() = default;

    int TurnOver;

    virtual const Spell* ChooseBestSpell(double currentMana) const;
    virtual const std::vector<Spell> &GetSpells() const;

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
private:
    std::vector<Spell> UpgradedSpells;

public:
    Intelligent();
    void UpgradeSpells();
    const std::vector<Spell> &GetSpells() const override;
};

class MainCharacter : public AI
{
public:
    MainCharacter();
    void updateSpellStats(int playerLevel);
    const std::vector<Spell>& GetSpells() const override;

protected:
    std::vector<Spell> HeroSpells;
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
