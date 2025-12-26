#include "AI.h"
#include "Unit.h"
#include "GameConstants.h"
#include "RandomGenerator.h"

AI::AI() : TurnOver(0) {}

void AI::InitializeSpells(int level, bool isHero, const std::vector<std::string>& heroSpells) {
    Spells.clear();
    if (isHero) {
        Spells = SpellManager::Instance().GetHeroSpellsByIds(heroSpells, level);
    } else {
        Spells = SpellManager::Instance().GetEnemySpellsForLevel(level);
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

void AI::ApplyMultipliers(const QMap<SpellType, double>& multipliers)
{
    for (auto &spell : Spells) {
        if (multipliers.contains(spell.type)) {
            double mult = multipliers.value(spell.type);
            spell.damage *= mult;
        }
    }
}

void AI::ApplyManaReductions(const QMap<SpellType, double>& reductions)
{
    for (auto &spell : Spells) {
        if (reductions.contains(spell.type)) {
            double reductionPercent = reductions.value(spell.type);
            spell.manacost = spell.manacost * (1.0 - reductionPercent);

            if (spell.manacost < 1.0) spell.manacost = 1.0;
        }
    }
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
}

void Intelligent::UpgradeSpells()
{
    for (auto &spell : Spells) {
        spell.damage *= GlobalConst::AI::INTEL_DMG_MULT;
        spell.manacost *= GlobalConst::AI::INTEL_MANA_MULT;
    }
}

MainCharacter::MainCharacter()
{
    this->TurnOver = GlobalConst::AI::INIT_HERO;
}

void MainCharacter::updateSpellStats(int playerLevel, const std::vector<std::string>& heroSpells) {
    InitializeSpells(playerLevel, true, heroSpells);
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
