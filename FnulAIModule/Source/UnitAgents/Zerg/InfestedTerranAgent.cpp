#include <UnitAgents\Zerg\InfestedTerranAgent.h>
#include <PotentialFields\PFManager.h>
#include <MainAgents\TargetingAgent.h>

InfestedTerranAgent::InfestedTerranAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "InfestedTerranAgent";
	//Broodwar->printf("InfestedTerranAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void InfestedTerranAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
