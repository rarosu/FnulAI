#include <StructureAgents\RefineryAgent.h>
#include <MainAgents\WorkerAgent.h>
#include <Managers\AgentManager.h>

RefineryAgent::RefineryAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "RefineryAgent";
}

void RefineryAgent::computeActions()
{
	for (int i = 0; i < (int)assignedWorkers.size(); i++)
	{
		if (!assignedWorkers.at(i)->isAlive())
		{
			assignedWorkers.erase(assignedWorkers.begin() + i);
			return;
		}
		if (!assignedWorkers.at(i)->getUnit()->isGatheringGas())
		{
			assignedWorkers.erase(assignedWorkers.begin() + i);
			return;
		}
	}

	if ((int)assignedWorkers.size() <= 3)
	{
		if (!unit->isBeingConstructed())
		{
			WorkerAgent* worker = (WorkerAgent*)AgentManager::Instance().findClosestFreeWorker(unit->getTilePosition());
			if (worker != NULL)
			{
				worker->getUnit()->rightClick(unit);
				worker->setState(WorkerAgent::GATHER_GAS);
				assignedWorkers.push_back(worker);
			}
		}
	}
}
