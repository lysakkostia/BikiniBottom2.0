#ifndef SKILLTREESYSTEM_H
#define SKILLTREESYSTEM_H

#include <QString>
#include <string>
#include <vector>
#include <map>
#include "SpellManager.h"

enum class SkillEffect {
    UnlockSpell,
    ReduceManaCost,
    IncreaseDamage
};

struct SkillNode {
    std::string id;
    QString name;
    QString description;

    SpellType branchType;
    int tier;

    SkillEffect effect;
    double value;
    std::string spellId;

    int cost;
    std::string parentId;

    SkillNode(std::string _id, QString _name, SpellType _type, int _tier, SkillEffect _eff, double _val, int _cost, std::string _parent = "", std::string _spell = "")
        : id(_id), name(_name), branchType(_type), tier(_tier), effect(_eff), value(_val), cost(_cost), parentId(_parent), spellId(_spell) {}
};

class SkillTreeData {
public:
    static std::vector<SkillNode> getAllNodes() {
        std::vector<SkillNode> nodes;

        // ================= FIRE BRANCH =================
        nodes.push_back(SkillNode("fire_1", "Pyromancy", SpellType::Fire, 1, SkillEffect::ReduceManaCost, 0.10, 1, "root"));
        nodes.push_back(SkillNode("fire_2", "Searing Heat", SpellType::Fire, 2, SkillEffect::IncreaseDamage, 0.10, 1, "fire_1"));
        nodes.push_back(SkillNode("fire_3", "Unlock: Magma", SpellType::Fire, 3, SkillEffect::UnlockSpell, 0, 2, "fire_2", "magma_eruption"));
        nodes.push_back(SkillNode("fire_4", "Volcanic Rage", SpellType::Fire, 4, SkillEffect::IncreaseDamage, 0.15, 2, "fire_3"));
        nodes.push_back(SkillNode("fire_5", "Unlock: Meteor", SpellType::Fire, 5, SkillEffect::UnlockSpell, 0, 3, "fire_4", "meteor"));
        nodes.push_back(SkillNode("fire_6", "Ult: Armageddon", SpellType::Fire, 6, SkillEffect::UnlockSpell, 0, 4, "fire_5", "armageddon"));

        // ================= ICE BRANCH =================
        nodes.push_back(SkillNode("ice_1", "Cryomancy", SpellType::Ice, 1, SkillEffect::ReduceManaCost, 0.10, 1, "root"));
        nodes.push_back(SkillNode("ice_2", "Deep Freeze", SpellType::Ice, 2, SkillEffect::IncreaseDamage, 0.10, 1, "ice_1"));
        nodes.push_back(SkillNode("ice_3", "Unlock: Frost Nova", SpellType::Ice, 3, SkillEffect::UnlockSpell, 0, 2, "ice_2", "frost_nova"));
        nodes.push_back(SkillNode("ice_4", "Glacial Efficiency", SpellType::Ice, 4, SkillEffect::ReduceManaCost, 0.15, 2, "ice_3"));
        nodes.push_back(SkillNode("ice_5", "Ult: Absolute Zero", SpellType::Ice, 5, SkillEffect::UnlockSpell, 0, 4, "ice_4", "absolute_zero"));

        // ================= ELECTRIC BRANCH =================
        nodes.push_back(SkillNode("elec_1", "Static Field", SpellType::Electric, 1, SkillEffect::ReduceManaCost, 0.10, 1, "root"));
        nodes.push_back(SkillNode("elec_2", "Overload", SpellType::Electric, 2, SkillEffect::IncreaseDamage, 0.10, 1, "elec_1"));
        nodes.push_back(SkillNode("elec_3", "Unlock: Chain L.", SpellType::Electric, 3, SkillEffect::UnlockSpell, 0, 2, "elec_2", "chain_lightning"));
        nodes.push_back(SkillNode("elec_4", "High Voltage", SpellType::Electric, 4, SkillEffect::IncreaseDamage, 0.20, 2, "elec_3"));
        nodes.push_back(SkillNode("elec_5", "Ult: Plasma Cannon", SpellType::Electric, 5, SkillEffect::UnlockSpell, 0, 4, "elec_4", "plasma_cannon"));

        // ================= POISON BRANCH =================
        nodes.push_back(SkillNode("pois_1", "Toxicology", SpellType::Poison, 1, SkillEffect::ReduceManaCost, 0.15, 1, "root"));
        nodes.push_back(SkillNode("pois_2", "Potent Venom", SpellType::Poison, 2, SkillEffect::IncreaseDamage, 0.10, 1, "pois_1"));
        nodes.push_back(SkillNode("pois_3", "Unlock: Acid Spray", SpellType::Poison, 3, SkillEffect::UnlockSpell, 0, 2, "pois_2", "acid_spray"));
        nodes.push_back(SkillNode("pois_4", "Unlock: Venom Nova", SpellType::Poison, 4, SkillEffect::UnlockSpell, 0, 3, "pois_3", "venom_nova"));
        nodes.push_back(SkillNode("pois_5", "Plague Lord", SpellType::Poison, 5, SkillEffect::IncreaseDamage, 0.30, 3, "pois_4"));

        // ================= DARK BRANCH =================
        nodes.push_back(SkillNode("dark_1", "Shadow Arts", SpellType::Dark, 1, SkillEffect::ReduceManaCost, 0.10, 1, "root"));
        nodes.push_back(SkillNode("dark_2", "Nightmare", SpellType::Dark, 2, SkillEffect::IncreaseDamage, 0.10, 1, "dark_1"));
        nodes.push_back(SkillNode("dark_3", "Unlock: Void Crush", SpellType::Dark, 3, SkillEffect::UnlockSpell, 0, 2, "dark_2", "void_crush"));
        nodes.push_back(SkillNode("dark_4", "Abyss Walker", SpellType::Dark, 4, SkillEffect::ReduceManaCost, 0.15, 2, "dark_3"));
        nodes.push_back(SkillNode("dark_5", "Ult: Soul Reap", SpellType::Dark, 5, SkillEffect::UnlockSpell, 0, 4, "dark_4", "soul_reap"));

        // ================= HOLY BRANCH =================
        nodes.push_back(SkillNode("holy_1", "Devotion", SpellType::Holy, 1, SkillEffect::ReduceManaCost, 0.10, 1, "root"));
        nodes.push_back(SkillNode("holy_2", "Smite Evil", SpellType::Holy, 2, SkillEffect::IncreaseDamage, 0.10, 1, "holy_1"));
        nodes.push_back(SkillNode("holy_3", "Unlock: Purify", SpellType::Holy, 3, SkillEffect::UnlockSpell, 0, 2, "holy_2", "purify"));
        nodes.push_back(SkillNode("holy_4", "Unlock: Sunray", SpellType::Holy, 4, SkillEffect::UnlockSpell, 0, 3, "holy_3", "sunray"));
        nodes.push_back(SkillNode("holy_5", "Ult: Divine Wrath", SpellType::Holy, 5, SkillEffect::UnlockSpell, 0, 4, "holy_4", "divine_wrath"));

        return nodes;
    }
};

#endif
