#include <UnitAgents\Terran\WraithAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <MainAgents\TargetingAgent.h>

WraithAgent::WraithAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "WraithAgent";
	//Broodwar->printf("WraithAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void WraithAgent::computeActions()
{
	// Get all units in sight
	std::set<Unit*> enemy_set = unit->getUnitsInRadius(type.sightRange());
	// Put them in a vector so I can use Lars's nifty Collections
	std::vector<Unit*> enemy_vec(enemy_set.begin(), enemy_set.end());

	enemy_vec = getUnitsMatchingPredicate(enemy_vec, &Predicate::IsHostile());
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

		// Only cloak if enemy unit can attack this guy
		if (enemy_vec[i]->getType().airWeapon() != BWAPI::WeaponTypes::None)
			cloak = true;
	}

	if (cloak)
	{
		if (!unit->isCloaked()){
			if (unit->cloak())
				Broodwar->printf("Wraith successfully cloaked");}
	}
	// If unit shouldn't be cloaked but is anyway. This is for going out of cloak.
	else if (unit->isCloaked())
		unit->decloak();

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}

bool WraithAgent::shouldBeCloaked()
{
	BWAPI::Unit* target = TargetingAgent::findTarget(this);

	// Cloak if encountering an enemy with air weapons
	if (target != NULL)
	{
		if (target->getType().airWeapon() != BWAPI::WeaponTypes::None)
		{
			return true;
		}
	}
	
	
	return true;
}
