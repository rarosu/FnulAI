#include <UnitAgents\Terran\FirebatAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>
#include <MainAgents\TargetingAgent.h>

FirebatAgent::FirebatAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "FirebatAgent";
	//Broodwar->printf("FirebatAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void FirebatAgent::computeActions()
{
	// Check if enemy units are within attack range
	if (enemyUnitsWithinRange(type.groundWeapon().maxRange()) > 0)
	{
		// Check if unit's squad has medics in it
		std::vector<BaseAgent*> mySquad = Commander::Instance().getSquad(squadID)->getMembers();
		bool hasMedic = false;
		if (getAgentsMatchingPredicate(mySquad, &Predicate::IsType(BWAPI::UnitTypes::Terran_Medic)).size() > 0)
			hasMedic = true;
		
		// If there are medics and no stim is active, activate stim
		if (hasMedic && unit->getStimTimer() == 0)
			unit->useTech(BWAPI::TechTypes::Stim_Packs);
	}

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
