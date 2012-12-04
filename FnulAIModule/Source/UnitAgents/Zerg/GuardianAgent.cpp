#include <UnitAgents\Zerg\GuardianAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>

GuardianAgent::GuardianAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GuardianAgent";
	//Broodwar->printf("GuardianAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void GuardianAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
