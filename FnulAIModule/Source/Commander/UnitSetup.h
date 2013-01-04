#ifndef UNIT_SETUP_HPP
#define UNIT_SETUP_HPP

#include <map>
#include <MainAgents\Collection\UnitCollection.hpp>

class UnitSetup
{
public:
	void addRequirement(BWAPI::UnitType type, int count);
	void removeRequirement(BWAPI::UnitType type);
	int getRequirement(BWAPI::UnitType type);

	/** Check whether the given units satisfy this setup */
	bool isSatisfied(const UnitCollection& units) const;
private:
	typedef std::map<BWAPI::UnitType, int> RequirementMap;
	RequirementMap m_requirements;
};

#endif