#include <Commander\Squad.h>

Squad::Squad(int id, 
		Tactic* defaultTactic, 
		const std::string& name, 
		unsigned int priority, 
		unsigned int activePriority,
		bool required)
{
	m_id = id;
	m_priority = priority;
	m_activePriority = activePriority;
	m_active = false;
	m_name = name;
	m_required = required;

	m_tacticStack.push_back(defaultTactic);

	m_goal = BWAPI::TilePosition(-1, -1);
	m_goalSetFrame = 0;
	m_goalArrivedFrame = -1;
}

void Squad::computeActions()
{
	//m_tacticStack.back()->
}

UnitSetup& Squad::getSetup()
{
	return m_setup;
}

UnitCollection& Squad::getUnits()
{
	return m_units;
}

int Squad::getID() const
{
	return m_id;
}

int Squad::getPriority() const
{
	return m_priority;
}

int Squad::getActivePriority() const
{
	return m_activePriority;
}

const std::string& Squad::getName() const
{
	return m_name;
}

bool Squad::isRequired() const
{
	return m_required;
}


void Squad::pushTactic(Tactic* tactic)
{
	m_tacticStack.push_back(tactic);
}

void Squad::popTactic()
{
	m_tacticStack.pop_back();
}

void Squad::setDefaultTactic(Tactic* tactic)
{
	m_tacticStack[0] = tactic;
}

bool Squad::isActive() const
{
	return m_setup.isSatisfied(m_units);
}



bool Squad::needUnit(BWAPI::Unit* unit) const
{
	return m_setup.getRequirement(type) - m_units.getUnitsMatchingPredicate(&Predicate::IsOfType(type)).m_units.size();
}
