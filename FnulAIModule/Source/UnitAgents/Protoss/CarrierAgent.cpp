#include <UnitAgents\Protoss\CarrierAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>

CarrierAgent::CarrierAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "CarrierAgent";
	//Broodwar->printf("CarrierAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void CarrierAgent::computeActions()
{
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
