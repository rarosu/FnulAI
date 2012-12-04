#include <UnitAgents\Zerg\DefilerAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <MainAgents\TargetingAgent.h>

DefilerAgent::DefilerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DefilerAgent";
	//Broodwar->printf("DefilerAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void DefilerAgent::computeActions()
{
	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
