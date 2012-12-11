#include <UnitAgents\Terran\SiegeTankAgent.h>
#include <Managers\AgentManager.h>
#include <PotentialFields\PFManager.h>
#include <MainAgents\TargetingAgent.h>

SiegeTankAgent::SiegeTankAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "SiegeTankAgent";
	//Broodwar->printf("SiegeTankAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void SiegeTankAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
