#include <UnitAgents\Zerg\ZerglingAgent.h>
#include <PotentialFields\PFManager.h>
#include <MainAgents\TargetingAgent.h>

ZerglingAgent::ZerglingAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ZerglingAgent";
	//Broodwar->printf("ZerglingAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ZerglingAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
