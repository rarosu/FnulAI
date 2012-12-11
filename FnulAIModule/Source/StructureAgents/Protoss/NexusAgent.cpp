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
	if (AgentManager::Instance().countNoUnits(UnitTypes::Protoss_Nexus) == 0)
	{
		//We dont do this for the first Nexus.
		hasSentWorkers = true;
	}
	
	BuildPlanner::Instance().commandCenterBuilt();
}

void NexusAgent::computeActions()
{
	if (!hasSentWorkers)
	{
		if (!unit->isBeingConstructed())
		{
			sendWorkers();
			hasSentWorkers = true;

			BuildPlanner::Instance().addRefinery();

			if (AgentManager::Instance().countNoUnits(UnitTypes::Protoss_Forge) > 0)
			{
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Protoss_Pylon);
				BuildPlanner::Instance().addBuildingFirst(UnitTypes::Protoss_Photon_Cannon);
			}
		}
	}

	if (!unit->isIdle())
	{
		//Already doing something
		return;
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
