#include <UnitAgents\Terran\GhostAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <MainAgents\TargetingAgent.h>
#include <MainAgents\Collection\Predicate.hpp>

GhostAgent::GhostAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "GhostAgent";
	//Broodwar->printf("GhostAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void GhostAgent::computeActions()
{
	// Get all units in sight
	std::set<Unit*> enemy_set = unit->getUnitsInRadius(type.sightRange());
	// Put them in a vector so I can use Lars's nifty Collections
	std::vector<Unit*> enemy_vec(enemy_set.begin(), enemy_set.end());

	enemy_vec = getUnitsMatchingPredicate(enemy_vec, &Predicate::IsHostile());

	std::vector<Unit*> enemy_mec = getUnitsMatchingPredicate(enemy_vec, &Predicate::IsMech());
	Unit* lockdown_target = NULL;
	if (!enemy_mec.empty())
	{
		lockdown_target = enemy_mec.front();
		for (size_t i = 0; i < enemy_mec.size(); ++i)
		{
			if (enemy_mec[i]->getType().destroyScore() > lockdown_target->getType().destroyScore() &&
				!enemy_mec[i]->isLockedDown())
				lockdown_target = enemy_mec[i];
		}
	}
	if (lockdown_target != NULL) {
		if (unit->useTech(BWAPI::TechTypes::Lockdown, lockdown_target))
			Broodwar->printf("Lockdown used!"); }
		
	enemy_vec = getUnitsMatchingPredicate(enemy_vec, &Predicate::HasGroundWeapon());
	bool cloak = false;

	// This loop decides if unit should be cloaked or not
	for (size_t i = 0; i < enemy_vec.size(); ++i)
	{
		// Worthless to cloak if there's a detector nearby
		if (enemy_vec[i]->getType().isDetector())
		{
			cloak = false;
			break;
		}

		cloak = true;
	}

	if (cloak)
	{
		if (!unit->isCloaked()){
			if (unit->cloak())
				Broodwar->printf("Ghost successfully cloaked!");}
	}
	// If unit shouldn't be cloaked but is anyway. This is for going out of cloak.
	else if (unit->isCloaked())
		unit->decloak();

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
