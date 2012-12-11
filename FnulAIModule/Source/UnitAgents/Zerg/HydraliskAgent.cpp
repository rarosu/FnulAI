#include <UnitAgents\Zerg\HydraliskAgent.h>
#include <PotentialFields\PFManager.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>

HydraliskAgent::HydraliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "HydraliskAgent";
	//Broodwar->printf("HydraliskAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void HydraliskAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
