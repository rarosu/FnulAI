#include <StructureAgents\Zerg\HatcheryAgent.h>
#include <Managers\AgentManager.h>
#include <MainAgents\WorkerAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\BuildPlanner.h>
#include <Commander\Commander.h>
#include <Managers\UpgradesPlanner.h>
#include <Managers\ResourceManager.h>

HatcheryAgent::HatcheryAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::Instance().countNoBases() == 0)
	{
		//We dont do this for the first Nexus.
		hasSentWorkers = true;
	}
	if (mUnit->getType().getID() == UnitTypes::Zerg_Lair.getID())
	{
		//Upgrade. Dont send workers.
		hasSentWorkers = true;
	}
	if (mUnit->getType().getID() == UnitTypes::Zerg_Hive.getID())
	{
		//Upgrade. Dont send workers.
		hasSentWorkers = true;
	}
	
	agentType = "HatcheryAgent";
	BuildPlanner::Instance().commandCenterBuilt();
}

void HatcheryAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;
			BuildPlanner::Instance().addRefinery();

			/*if (AgentManager::Instance().countNoUnits(UnitTypes::Zerg_Spawning_Pool) > 0)
			{
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Zerg_Creep_Colony);
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Zerg_Sunken_Colony);
			}*/
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}
	
	//Build Overlords for supply
	if (BuildPlanner::Instance().shallBuildSupply())
	{
		if (canBuild(UnitTypes::Zerg_Overlord))
		{
			unit->train(UnitTypes::Zerg_Overlord);
			return;
		}
	}
	//Build units
	if (checkBuildUnit(UnitTypes::Zerg_Queen)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Mutalisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Hydralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Zergling)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Defiler)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Ultralisk)) return;
	if (checkBuildUnit(UnitTypes::Zerg_Scourge)) return;

	//Create workers
	if (ResourceManager::Instance().needWorker())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}

	//Check for base upgrades
	if (isOfType(UnitTypes::Zerg_Hatchery))
	{
		if (Broodwar->canMake(unit, UnitTypes::Zerg_Lair))
		{
			if (ResourceManager::Instance().hasResources(UnitTypes::Zerg_Lair))
			{
				ResourceManager::Instance().lockResources(UnitTypes::Zerg_Lair);
				unit->morph(UnitTypes::Zerg_Lair);
				return;
			}
		}
	}
	if (isOfType(UnitTypes::Zerg_Lair))
	{
		if (AgentManager::Instance().countNoUnits(UnitTypes::Zerg_Hive) < 2)
		{
			if (Broodwar->canMake(unit, UnitTypes::Zerg_Hive))
			{
				if (ResourceManager::Instance().hasResources(UnitTypes::Zerg_Hive))
				{
					ResourceManager::Instance().lockResources(UnitTypes::Zerg_Hive);
					unit->morph(UnitTypes::Zerg_Hive);
					return;
				}
			}
		}
	}
	

	//Check for upgrades
	UpgradesPlanner::Instance().checkUpgrade(this);
}

bool HatcheryAgent::checkBuildUnit(UnitType type)
{
	if (canEvolveUnit(type))
	{
		unit->train(type);
		return true;
	}
	return false;
}

