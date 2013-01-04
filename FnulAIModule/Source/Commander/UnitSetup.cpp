#include <Commander\UnitSetup.h>
#include <MainAgents\Collection\Predicate.hpp>

void UnitSetup::addRequirement(BWAPI::UnitType type, int count)
{
	m_requirements[type] = count;
}

void UnitSetup::removeRequirement(BWAPI::UnitType type)
{
	RequirementMap::iterator it = m_requirements.find(type);
	if (it != m_requirements.end())
		m_requirements.erase(it);
}

int UnitSetup::getRequirement(BWAPI::UnitType type)
{
	RequirementMap::iterator it = m_requirements.find(type);
	if (it != m_requirements.end())
		return it->second;
	return -1;
}

bool UnitSetup::isSatisfied(const UnitCollection& units) const
{
	for (RequirementMap::iterator it = m_requirements.begin(); it != m_requirements.end(); it++)
	{
		UnitCollection unitsOfType = units.getUnitsMatchingPredicate(&Predicate::IsOfType(it->first));
		if (unitsOfType.m_units.size() < it->second)
			return false;
	}

	return true;
}

UnitSetup UnitSetup::getRemainingRequiredUnits(const UnitCollection& units) const
{
	UnitSetup result;
	for (RequirementMap::iterator it = m_requirements.begin(); it != m_requirements.end(); it++)
	{
		UnitCollection unitsOfType = units.getUnitsMatchingPredicate(&Predicate::IsOfType(it->first));
		
		result.addRequirement(it->first, unitsOfType.m_units.size() - it->second);
	}

	return true;
}
