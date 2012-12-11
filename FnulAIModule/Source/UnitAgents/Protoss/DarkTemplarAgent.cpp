#include <UnitAgents\Protoss\DarkTemplarAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Managers\ExplorationManager.h>
#include <MainAgents\TargetingAgent.h>

DarkTemplarAgent::DarkTemplarAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "DarkTemplarAgent";
	//Broodwar->printf("DarkTemplarAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void DarkTemplarAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
