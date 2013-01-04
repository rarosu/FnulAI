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
	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);

	// Cloak if encountering an enemy that has air attacks, stay uncloaked otherwise
	if (shouldBeCloaked())
		this->issueCommand(BWAPI::UnitCommandTypes::Cloak, this->getUnit());
	else
		this->issueCommand(BWAPI::UnitCommandTypes::Decloak, this->getUnit());	
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
