#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <BWAPI.h>

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

	class HasTech : public Predicate
	{
	public:
		HasTech(const BWAPI::TechType& tech);

		bool Evaluate(BWAPI::Unit* unit);
	private:
		BWAPI::TechType m_tech;
	};
}



#endif