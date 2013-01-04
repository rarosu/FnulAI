#include <MainAgents\Collection\Predicate.hpp>

namespace Predicate
{
	bool IsAlive::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
			return true;
		return false;
	}


	bool IsUnit::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return !unit->getType().isBuilding();
		}

		// Agent is deleted
		return false;
	}


	bool IsWorker::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType().isWorker();
		}

		// Agent is deleted
		return false;
	}


	bool IsBuilding::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType().isBuilding();
		}

		// Agent is deleted
		return false;
	}


	bool IsAir::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType().isFlyer();
		}

		// Agent is deleted
		return false;
	}


	bool IsGround::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return !unit->getType().isFlyer();
		}

		// Agent is deleted
		return false;
	}


	HasTech::HasTech(const BWAPI::TechType& tech)
		: m_tech(tech)
	{}

	bool HasTech::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			const std::set<BWAPI::TechType>& abilities = unit->getType().abilities();
			
			std::set<BWAPI::TechType>::const_iterator ability;
			for (ability = abilities.begin(); ability != abilities.end(); ability++)
			{
				if ((*ability) == m_tech)
					return true;
			}

			return false;
		}

		// Agent is deleted
		return false;
	}
}

