#ifndef UNIT_COLLECTION_HPP
#define UNIT_COLLECTION_HPP

#include <BWAPI.h>
#include <MainAgents\BaseAgent.h>
#include <MainAgents\Collection\Predicate.hpp>

class UnitCollection
{
public:
	/** Collection of unit references */
	std::vector<BWAPI::Unit*> m_units;

	/** Select a subset of units */
	UnitCollection getUnitsMatchingPredicate(Predicate::Predicate* predicate) const;

	/** Get the units that are agents only */
	std::vector<BaseAgent*> getAgents() const;

	/** Get collection metadata */
	BWAPI::Position getCenter() const;
};

#endif