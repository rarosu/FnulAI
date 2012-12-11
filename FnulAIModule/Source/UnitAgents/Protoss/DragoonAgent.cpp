#include <UnitAgents\Protoss\DragoonAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>
#include <Utils\Profiler.h>

DragoonAgent::DragoonAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DragoonAgent";
	//Broodwar->printf("DragoonAgent created (%s)", unit->getType().getName().c_str());
	goal = TilePosition(-1, -1);
}

void DragoonAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
