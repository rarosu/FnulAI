#include <UnitAgents\Protoss\HighTemplarAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>

HighTemplarAgent::HighTemplarAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "HighTemplarAgent";
	//Broodwar->printf("HighTemplarAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void HighTemplarAgent::computeActions()
{
	bool defensive = true;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}

