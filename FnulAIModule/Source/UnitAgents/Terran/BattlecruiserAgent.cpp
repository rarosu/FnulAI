#include <UnitAgents\Terran\BattlecruiserAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <MainAgents\TargetingAgent.h>

BattlecruiserAgent::BattlecruiserAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "BattlecruiserAgent";
	//Broodwar->printf("BattlecruiserAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void BattlecruiserAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
