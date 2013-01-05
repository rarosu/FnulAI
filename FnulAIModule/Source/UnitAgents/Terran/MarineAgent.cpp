#include <UnitAgents\Terran\MarineAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>
#include <MainAgents\TargetingAgent.h>
#include <MainAgents\Collection\Predicate.hpp>

MarineAgent::MarineAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MarineAgent";
	//Broodwar->printf("MarineAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

bool MarineAgent::isNeededInBunker()
{
	if (!unit->isLoaded())
	{
		Squad* sq = Commander::Instance().getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->isBunkerDefend())
			{
				vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					if (agents.at(i)->isAlive() && agents.at(i)->isOfType(UnitTypes::Terran_Bunker))
					{
						if (agents.at(i)->getUnit()->getLoadedUnits().size() < 4)
						{
							unit->rightClick(agents.at(i)->getUnit());
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

void MarineAgent::computeActions()
{
	if (isNeededInBunker()) return;
	
	// Check if enemy units are within attack range
	if (enemyUnitsWithinRange(type.groundWeapon().maxRange()) > 0)
	{
		// Check if unit's squad has medics in it
		std::vector<BaseAgent*> mySquad = Commander::Instance().getSquad(squadID)->getMembers();
		bool hasMedic = false;
		if (getAgentsMatchingPredicate(mySquad, &Predicate::IsType(BWAPI::UnitTypes::Terran_Medic)).size() > 0)
			hasMedic = true;
		
		// If there are medics and no stim is active, activate stim
		if (hasMedic && unit->getStimTimer() == 0)
			unit->useTech(BWAPI::TechTypes::Stim_Packs)
	}

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
