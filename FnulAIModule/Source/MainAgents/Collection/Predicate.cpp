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

	bool IsHostile::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return BWAPI::Broodwar->enemies().find(unit->getPlayer()) != BWAPI::Broodwar->enemies().end();
		}

		return false;
	}

	bool IsFriendly::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return BWAPI::Broodwar->allies().find(unit->getPlayer()) != BWAPI::Broodwar->allies().end();
		}

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
		}

		return false;
	}

	IsType::IsType(const BWAPI::UnitType& type)
		: m_type(type)
	{}

	bool IsType::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0 && unit->getType().getID() == m_type.getID())
			return true;

		return false;
	}

	bool HasGroundWeapon::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType().groundWeapon() != BWAPI::WeaponTypes::None;
		}

		return false;
	}
			
	bool HasAirWeapon::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType().airWeapon() != BWAPI::WeaponTypes::None;
		}

		return false;
	}

	bool IsCommandCenter::Evaluate(BWAPI::Unit* unit)
	{
		if (unit != NULL && unit->getHitPoints() > 0)
		{
			return unit->getType() == BWAPI::UnitTypes::Terran_Command_Center;
		}

		return false;
	}
}


std::vector<BWAPI::Unit*> getUnitsMatchingPredicate(const std::vector<BWAPI::Unit*>& units, Predicate::Predicate* predicate)
{
	std::vector<BWAPI::Unit*> result;

	for (size_t i = 0; i < units.size(); ++i)
	{
		if (predicate->Evaluate(units[i]))
		{
			result.push_back(units[i]);
		}
	}

	return result;
}

std::vector<BaseAgent*> getAgentsMatchingPredicate(const std::vector<BaseAgent*>& agents, Predicate::Predicate* predicate)
{
	std::vector<BaseAgent*> result;

	for (size_t i = 0; i < agents.size(); ++i)
	{
		if (predicate->Evaluate(agents[i]->getUnit()))
		{
			result.push_back(agents[i]);
		}
	}

	return result;
}

