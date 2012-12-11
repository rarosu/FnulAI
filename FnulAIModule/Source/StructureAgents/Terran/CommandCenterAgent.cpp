#include <StructureAgents\Terran\CommandCenterAgent.h>
#include <Managers\AgentManager.h>
#include <MainAgents\WorkerAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\BuildPlanner.h>
#include <Managers\ExplorationManager.h>
#include <Managers\ResourceManager.h>

CommandCenterAgent::CommandCenterAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::Instance().countNoUnits(UnitTypes::Terran_Command_Center) == 0)
	{
		//We dont do this for the first Command Center.
		hasSentWorkers = true;
	}

	agentType = "CommandCenterAgent";
	BuildPlanner::Instance().commandCenterBuilt();
}

void CommandCenterAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;

			BuildPlanner::Instance().addRefinery();

			if (AgentManager::Instance().countNoUnits(UnitTypes::Terran_Barracks) > 0)
			{
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Terran_Bunker);
			}
			if (AgentManager::Instance().countNoUnits(UnitTypes::Terran_Engineering_Bay) > 0)
			{
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Terran_Missile_Turret);
			}
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}

	//Build comsat addon
	if (unit->getAddon() == NULL)
	{
		if (Broodwar->canMake(unit, UnitTypes::Terran_Comsat_Station))
		{
			unit->buildAddon(UnitTypes::Terran_Comsat_Station);
			return;
		}
	}

	if (ResourceManager::Instance().needWorker())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
}
