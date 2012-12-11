#include <UnitAgents\Protoss\ReaverAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <MainAgents\TargetingAgent.h>

ReaverAgent::ReaverAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ReaverAgent";
	//Broodwar->printf("ReaverAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ReaverAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
