#include <Commander\Squad.hh>

Squad::Squad(int id, 
		Tactic* defaultTactic, 
		const std::string& name, 
		unsigned int priority, 
		unsigned int activePriority)
{
	m_id = id;
	m_priority = priority;
	m_activePriority = activePriority;
	m_active = false;
	m_name = name;

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



void Squad::addMember(BaseAgent* agent)
{
	m_units.m_units.push_back(agent->getUnit());
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

bool Squad::isRequired() const
{
	return m_required;
}

bool Squad::needUnit(const BWAPI::UnitType& type) const
{
	return m_setup.getRequirement(type) - m_units.getUnitsMatchingPredicate(&Predicate::IsOfType(type)).m_units.size();
}
