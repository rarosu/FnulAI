#include <UnitAgents\Zerg\MutaliskAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Squad.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>

MutaliskAgent::MutaliskAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "MutaliskAgent";
	//Broodwar->printf("MutaliskAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void MutaliskAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
