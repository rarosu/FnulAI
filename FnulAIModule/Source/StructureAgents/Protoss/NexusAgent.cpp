#include <StructureAgents\Protoss\NexusAgent.h>
#include <Managers\AgentManager.h>
#include <MainAgents\WorkerAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\BuildPlanner.h>
#include <Managers\ResourceManager.h>

NexusAgent::NexusAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "NexusAgent";
	//Broodwar->printf("New base created at (%d,%d)", unit->getTilePosition().x(), unit->getTilePosition().y());

	hasSentWorkers = false;
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Protoss_Nexus) == 0)
	{
		//We dont do this for the first Nexus.
		hasSentWorkers = true;
	}
	
	BuildPlanner::getInstance()->commandCenterBuilt();
}

void NexusAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;

			BuildPlanner::getInstance()->addRefinery();

			if (AgentManager::getInstance()->countNoUnits(UnitTypes::Protoss_Forge) > 0)
			{
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Protoss_Pylon);
				BuildPlanner::getInstance()->addBuildingFirst(UnitTypes::Protoss_Photon_Cannon);
			}
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
	}

	if (ResourceManager::getInstance()->needWorker())
	{
		UnitType worker = Broodwar->self()->getRace().getWorker();
		if (canBuild(worker))
		{
			unit->train(worker);
		}
	}
}
