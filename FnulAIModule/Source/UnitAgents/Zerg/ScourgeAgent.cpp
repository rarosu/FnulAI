#include <UnitAgents\Zerg\ScourgeAgent.h>
#include <PotentialFields\PFManager.h>
#include <MainAgents\TargetingAgent.h>

ScourgeAgent::ScourgeAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScourgeAgent";
	//Broodwar->printf("ScourgeAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ScourgeAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
