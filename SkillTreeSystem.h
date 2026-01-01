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

        // --- FIRE BRANCH ---
        nodes.push_back(SkillNode("fire_1", "Flame Control", SpellType::Fire, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("fire_2", "Burning Rage", SpellType::Fire, 2, SkillEffect::IncreaseDamage, 0.05, 1, "fire_1"));
        nodes.push_back(SkillNode("fire_3", "Learn: Magma", SpellType::Fire, 3, SkillEffect::UnlockSpell, 0, 2, "fire_2", "magma_eruption"));
        nodes.push_back(SkillNode("fire_4", "Learn: Meteor", SpellType::Fire, 4, SkillEffect::UnlockSpell, 0, 3, "fire_3", "meteor"));


        // --- ICE BRANCH ---
        nodes.push_back(SkillNode("ice_1", "Cold Mind", SpellType::Ice, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("ice_2", "Frostbite", SpellType::Ice, 2, SkillEffect::IncreaseDamage, 0.05, 1, "ice_1"));
        nodes.push_back(SkillNode("ice_3", "Learn: Frost Nova", SpellType::Ice, 3, SkillEffect::UnlockSpell, 0, 2, "ice_2", "frost_nova"));

        // --- ELECTRIC BRANCH ---
        nodes.push_back(SkillNode("elec_1", "Conductivity", SpellType::Electric, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("elec_2", "High Voltage", SpellType::Electric, 2, SkillEffect::IncreaseDamage, 0.05, 1, "elec_1"));
        nodes.push_back(SkillNode("elec_3", "Learn: Chain Lightning", SpellType::Electric, 3, SkillEffect::UnlockSpell, 0, 2, "elec_2", "chain_lightning"));

        // --- POISON BRANCH ---
        nodes.push_back(SkillNode("pois_1", "Toxicology", SpellType::Poison, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("pois_2", "Potent Venom", SpellType::Poison, 2, SkillEffect::IncreaseDamage, 0.05, 1, "pois_1"));
        nodes.push_back(SkillNode("pois_3", "Learn: Acid Spray", SpellType::Poison, 3, SkillEffect::UnlockSpell, 0, 2, "pois_2", "acid_spray"));

        // --- DARK BRANCH ---
        nodes.push_back(SkillNode("dark_1", "Shadow Affinity", SpellType::Dark, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("dark_2", "Dark Power", SpellType::Dark, 2, SkillEffect::IncreaseDamage, 0.05, 1, "dark_1"));
        nodes.push_back(SkillNode("dark_3", "Learn: Void Crush", SpellType::Dark, 3, SkillEffect::UnlockSpell, 0, 2, "dark_2", "void_crush"));

        // --- HOLY BRANCH ---
        nodes.push_back(SkillNode("holy_1", "Devotion", SpellType::Holy, 1, SkillEffect::ReduceManaCost, 0.05, 1, "root"));
        nodes.push_back(SkillNode("holy_2", "Divine Light", SpellType::Holy, 2, SkillEffect::IncreaseDamage, 0.05, 1, "holy_1"));
        nodes.push_back(SkillNode("holy_3", "Learn: Divine Wrath", SpellType::Holy, 3, SkillEffect::UnlockSpell, 0, 2, "holy_2", "divine_wrath"));

        return nodes;
    }
};

#endif
