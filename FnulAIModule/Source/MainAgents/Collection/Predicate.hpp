#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <BWAPI.h>
#include <vector>
#include <MainAgents\BaseAgent.h>

namespace Predicate
{
	/** Abstract predicate class for selecting subsets of units */
	class Predicate
	{
	public:
		virtual bool Evaluate(BWAPI::Unit* unit) = 0;
	};


	/** Predefined predicates */
	class IsAlive : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsUnit: public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsWorker : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsBuilding : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsAir : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsGround : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsHostile : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsFriendly : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsMech : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class HasTech : public Predicate
	{
	public:
		HasTech(const BWAPI::TechType& tech);

		bool Evaluate(BWAPI::Unit* unit);
	private:
		BWAPI::TechType m_tech;
	};

	class IsType : public Predicate
	{
	public:
		IsType(const BWAPI::UnitType& type);

		bool Evaluate(BWAPI::Unit* unit);
	private:
		BWAPI::UnitType m_type;
	};

	class HasGroundWeapon : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class HasAirWeapon : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};

	class IsCommandCenter : public Predicate
	{
	public:
		bool Evaluate(BWAPI::Unit* unit);
	};
}

std::vector<BWAPI::Unit*> getUnitsMatchingPredicate(const std::vector<BWAPI::Unit*>& units, Predicate::Predicate* predicate);
std::vector<BaseAgent*> getAgentsMatchingPredicate(const std::vector<BaseAgent*>& agents, Predicate::Predicate* predicate);

#endif