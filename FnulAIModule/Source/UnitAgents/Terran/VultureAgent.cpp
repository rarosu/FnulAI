#include <UnitAgents\Terran\VultureAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>

VultureAgent::VultureAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "VultureAgent";
	//Broodwar->printf("VultureAgent created (%s)", unit->getType().getName().c_str());

	goal = TilePosition(-1, -1);
}

void VultureAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
