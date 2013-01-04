#include <MainAgents\Collection\UnitCollection.hpp>
#include <Managers\AgentManager.h>

UnitCollection UnitCollection::getUnitsMatchingPredicate(Predicate::Predicate* predicate) const
{
	UnitCollection result;

	for (size_t i = 0; i < m_units.size(); ++i)
	{
		if (predicate->Evaluate(m_units[i]))
		{
			result.m_units.push_back(m_units[i]);
		}
	}

	return result;
}

std::vector<BaseAgent*> UnitCollection::getAgents() const
{
	std::vector<BaseAgent*> agents;

	for (size_t i = 0; i < m_units.size(); ++i)
	{
		agents.push_back(AgentManager::Instance().getAgent(m_units[i]));
	}

	return agents;
}

BWAPI::Position UnitCollection::getCenter() const
{
	float x = 0.0f;
	float y = 0.0f;

	for (size_t i = 0; i < m_units.size(); ++i)
	{
		BWAPI::Position position = m_units[i]->getPosition();
		x += position.x();
		y += position.y();
	}

	x /= m_units.size();
	y /= m_units.size();

	return BWAPI::Position(static_cast<int>(x), static_cast<int>(y));
}