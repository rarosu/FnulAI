#include <UnitAgents\Zerg\QueenAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>

QueenAgent::QueenAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "QueenAgent";
	//Broodwar->printf("QueenAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void QueenAgent::computeActions()
{
	bool defensive = true;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}

