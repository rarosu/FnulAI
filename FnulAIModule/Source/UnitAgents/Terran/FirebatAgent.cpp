#include <UnitAgents\Terran\FirebatAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>
#include <MainAgents\TargetingAgent.h>

FirebatAgent::FirebatAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "FirebatAgent";
	//Broodwar->printf("FirebatAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void FirebatAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
