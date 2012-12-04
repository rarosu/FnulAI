#include <UnitAgents\Protoss\ArbiterAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>

ArbiterAgent::ArbiterAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ArbiterAgent";
	//Broodwar->printf("ArbiterAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ArbiterAgent::computeActions()
{
	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
