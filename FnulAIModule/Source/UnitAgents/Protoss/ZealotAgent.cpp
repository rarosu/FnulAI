#include <UnitAgents\Protoss\ZealotAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>
#include <MainAgents\TargetingAgent.h>
#include <Utils\Profiler.h>

ZealotAgent::ZealotAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ZealotAgent";
	//Broodwar->printf("ZealotAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ZealotAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
