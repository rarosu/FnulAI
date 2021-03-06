#include <UnitAgents\Protoss\ObserverAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>

ObserverAgent::ObserverAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ObserverAgent";
	//Broodwar->printf("ObserverAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void ObserverAgent::computeActions()
{
	bool defensive = true;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
