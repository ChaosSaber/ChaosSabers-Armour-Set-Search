#include "gear/SkillType.hpp"

std::string Gear::FreeCellSlotToStringKey(SkillType type)
{
    switch (type)
    {
    case Power: return "free_power_cell_slot"; break;
    case Technique: return "free_technique_cell_slot"; break;
    case Mobility: return "free_mobility_cell_slot"; break;
    case Defense: return "free_defense_cell_slot"; break;
    case Utility: return "free_utility_cell_slot"; break;
    default: return "unknown_cell_slot"; break;
    }
}

std::string Gear::SkillTypeToStringKey(SkillType type)
{
    switch (type)
    {
    case Power: return "power"; break;
    case Technique: return "technique"; break;
    case Mobility: return "mobility"; break;
    case Defense: return "defense"; break;
    case Utility: return "utility"; break;
    default: return "unknown"; break;
    }
}

std::string Gear::cellSlotToStringKey(SkillType type)
{
    switch (type)
    {
    case Power: return "cell_slot_power"; break;
    case Technique: return "cell_slot_technique"; break;
    case Mobility: return "cell_slot_mobility"; break;
    case Defense: return "cell_slot_defense"; break;
    case Utility: return "cell_slot_utility"; break;
    default: return "unknown"; break;
    }
}
