#include <UnitAgents\Terran\MarineAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>
#include <MainAgents\TargetingAgent.h>

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
	
	if (enemyUnitsWithinRange(type.groundWeapon().maxRange) > 0)
	{
		std::vector<BaseAgent*> mySquad = Commander::Instance().getSquad(squadID)->getMembers();
		bool hasMedic = false;
		for (size_t i = 0; i < mySquad.size(); ++i)
		{
			if (mySquad[i]->getUnitType().getName() == "Terran Medic")
			{
				hasMedic = true;
				break;
			}
		}

		if (hasMedic && unit->getStimTimer() == 0)
			unit->useTech(BWAPI::TechTypes::Stim_Packs);
	}

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
