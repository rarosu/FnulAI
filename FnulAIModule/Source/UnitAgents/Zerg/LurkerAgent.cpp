#include <UnitAgents\Zerg\LurkerAgent.h>
#include <PotentialFields\PFManager.h>
#include <Commander\Commander.h>
#include <Managers\ExplorationManager.h>
#include <MainAgents\TargetingAgent.h>

LurkerAgent::LurkerAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "LurkerAgent";
	//Broodwar->printf("LurkerAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void LurkerAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
