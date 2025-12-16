#include "AI.h"
#include "Unit.h"
#include "GameConstants.h"
#include "RandomGenerator.h"

Spell::Spell(std::string name, double manacost, double damage)
    : name(name), manacost(manacost), damage(damage)
{}

AI::AI() : TurnOver(0)
{
    for (const auto& data : GlobalConst::Spells::BaseSpells) {
        if (data.name == "TestImba bolt") continue;
        Spells.emplace_back(data.name, data.manaCost, data.damage);
    }
}

const std::vector<Spell>& AI::GetSpells() const
{
    return Spells;
}

const Spell* AI::ChooseBestSpell(double currentMana) const
{
    const Spell* bestSpell = nullptr;
    double maxDamage = -1.0;

    for (const Spell& spell : GetSpells()) {
        if (spell.manacost <= currentMana) {
            if (spell.damage > maxDamage) {
                maxDamage = spell.damage;
                bestSpell = &spell;
            }
        }
    }
    return bestSpell;
}

Aggresive::Aggresive()
{
    this->TurnOver = GlobalConst::AI::INIT_AGGRESSIVE;
}

Confused::Confused()
{
    this->TurnOver = GlobalConst::AI::INIT_CONFUSED;
}

const Spell* Confused::ChooseBestSpell(double currentMana) const
{
    const std::vector<Spell>& allSpells = this->GetSpells();
    std::vector<const Spell*> availableSpellPointers;

    for (const Spell& spell : allSpells) {
        if (spell.manacost <= currentMana) {
            availableSpellPointers.push_back(&spell);
        }
    }

    if (!availableSpellPointers.empty()) {
        int randomIndex = RandGenerator::RandIntInInterval(0, availableSpellPointers.size() - 1);
        return availableSpellPointers[randomIndex];
    }

    return nullptr;
}

Intelligent::Intelligent()
{
    this->TurnOver = GlobalConst::AI::INIT_INTELLIGENT;
    this->UpgradeSpells();
}

void Intelligent::UpgradeSpells()
{
     this->UpgradedSpells = this->Spells;
    for (auto &Spell : UpgradedSpells) {
        Spell.damage *= GlobalConst::AI::INTEL_DMG_MULT;
        Spell.manacost *= GlobalConst::AI::INTEL_MANA_MULT;
    }
}

const std::vector<Spell> &Intelligent::GetSpells() const
{
    return UpgradedSpells;
}

MainCharacter::MainCharacter()
{
    this->TurnOver = GlobalConst::AI::INIT_HERO;
    updateSpellStats(1);
}

void MainCharacter::updateSpellStats(int playerLevel) {
    HeroSpells.clear();

    int levelsAboveOne = (playerLevel > 1) ? (playerLevel - 1) : 0;

    for (const auto& baseDef : GlobalConst::Spells::BaseSpells) {
        double damageMultiplier = 1.0 + (baseDef.dmgGrowth * levelsAboveOne);
        double currentDamage = baseDef.damage * damageMultiplier;

        double manacostMultiplier = 1.0 + (baseDef.manaGrowth * levelsAboveOne);
        double currentManacost = baseDef.manaCost * manacostMultiplier;

        if (baseDef.manaGrowth < 0) {
            double minManacostFromBase = baseDef.manaCost * 0.30;
            if (currentManacost < minManacostFromBase) {
                currentManacost = minManacostFromBase;
            }
        }
        if (currentManacost < 1.0 && baseDef.manaCost > 0) currentManacost = 1.0;
        if (baseDef.manaCost > 0 && currentManacost <= 0) currentManacost = 1.0;
        if (currentDamage < 0) currentDamage = 0;

        HeroSpells.emplace_back(baseDef.name, currentManacost, currentDamage);
    }
}

const std::vector<Spell>& MainCharacter::GetSpells() const {
    return HeroSpells;
}

Friendly::Friendly()
{
    this->TurnOver = 0;
}

std::string Friendly::getGreeting() const
{
    return "Greetings, traveler! I am glad to see you in our valley, but be careful, as there is much danger here.";
}

Campfire::Campfire()
{
    this->TurnOver = 0;
}

void Campfire::Heal(Unit* target) {
    if (target) {
        target->SetHP(target->GetMaxHP());
        target->SetMana(target->GetMaxMana());
    }
}
