#include <UnitAgents\Terran\SiegeTankAgent.h>
#include <Managers\AgentManager.h>
#include <PotentialFields\PFManager.h>
#include <MainAgents\TargetingAgent.h>
#include <MainAgents\Collection\Predicate.hpp>
#include <Commander\Squad.h>
#include <Commander\Commander.h>

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
	if (shouldBeInSiege())
	{
		if (!unit->isSieged())
		{
			unit->siege();
		}
	}
	else
	{
		if (unit->isSieged())
		{
			unit->unsiege();
		}
	}

	bool defensive = false;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}

bool SiegeTankAgent::shouldBeInSiege()
{
	bool inDefenseSquad = isInStationaryDefensiveSquad();
	bool threatened = isThreatened();
	bool hasTargets = hasTargetsInRange();

	/*
	if (inDefenseSquad)
		Broodwar->printf("Siege Tank %d is in defense squad", unit->getID());
	else
		Broodwar->printf("Siege Tank %d is not in defense squad", unit->getID());
	if (threatened)
		Broodwar->printf("Siege Tank %d is threatened!", unit->getID());
	if (hasTargets)
		Broodwar->printf("Siege Tank %d has targets.", unit->getID());
	*/

	// Never stay in siege if we are threatened
	if (threatened)
		return false;

	// Siege if we are in a defense squad
	if (inDefenseSquad)
		return true;

	// Otherwise, siege only if we have targets in range
	if (hasTargets)
		return true;
	
	// Otherwise, return false
	return false;
}

bool SiegeTankAgent::isInStationaryDefensiveSquad()
{
	Squad* sq = Commander::Instance().getSquad(squadID);
	if (sq->isDefensive() && sq->isCloseTo(sq->getCenter()))
		return true;
	return false;
}

bool SiegeTankAgent::isThreatened()
{
	std::vector<BWAPI::Unit*> units;
	std::set<BWAPI::Unit*> unitSet = unit->getUnitsInRadius(15);
	for (std::set<BWAPI::Unit*>::iterator it = unitSet.begin(); it != unitSet.end(); it++)
		units.push_back(*it);

	std::vector<BWAPI::Unit*> threats = units;
	threats = getUnitsMatchingPredicate(threats, &Predicate::IsHostile());
	threats = getUnitsMatchingPredicate(threats, &Predicate::HasGroundWeapon());

	if (threats.size() > 0)
	{
		std::vector<BWAPI::Unit*> allies = units;
		allies = getUnitsMatchingPredicate(allies, &Predicate::IsFriendly());
		
		bool requiresAirWeapon = false;
		bool requiresGroundWeapon = false;
		for (size_t i = 0; i < threats.size(); ++i)
		{
			if (threats[i]->getType().isFlyer()) 
				requiresAirWeapon = true;
			else								 
				requiresGroundWeapon = true;
		}

		bool hasAirWeapon = false;
		bool hasGroundWeapon = false;
		for (size_t i = 0; i < allies.size(); ++i)
		{
			if (allies[i]->getType().airWeapon() != BWAPI::WeaponTypes::None)
				hasAirWeapon = true;
			if (allies[i]->getType().groundWeapon() != BWAPI::WeaponTypes::None)
				hasGroundWeapon = true;
		}

		if (requiresAirWeapon && !hasAirWeapon)
			return true;
		if (requiresGroundWeapon && !hasGroundWeapon)
			return true;
	}

	return false;
}

bool SiegeTankAgent::hasTargetsInRange()
{
	int minRange = BWAPI::WeaponTypes::Arclite_Shock_Cannon.minRange();
	int maxRange = BWAPI::WeaponTypes::Arclite_Shock_Cannon.maxRange();

	std::vector<BWAPI::Unit*> units;
	//std::set<BWAPI::Unit*> unitSet = unit->getUnitsInRadius(maxRange);
	std::set<BWAPI::Unit*> unitSet = unit->getUnitsInWeaponRange(BWAPI::WeaponTypes::Arclite_Shock_Cannon);
	for (std::set<BWAPI::Unit*>::iterator it = unitSet.begin(); it != unitSet.end(); it++)
		units.push_back(*it);

	std::vector<BWAPI::Unit*> targets = units;
	targets = getUnitsMatchingPredicate(targets, &Predicate::IsHostile());
	targets = getUnitsMatchingPredicate(targets, &Predicate::IsGround());

	for (size_t i = 0; i < targets.size(); ++i)
	{
		double distance = unit->getDistance(targets[i]);
		if (distance > minRange &&
			distance < maxRange)
			return true;
	}

	return false;
}

