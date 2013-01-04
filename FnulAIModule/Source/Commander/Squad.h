#ifndef SQUAD_HPP
#define SQUAD_HPP

#include <vector>
#include <MainAgents\Collection\Predicate.hpp>
#include <MainAgents\Collection\UnitCollection.hpp>
#include <Commander\Tactic.hpp>
#include <Commander\UnitSetup.h>

class Squad
{
public:
	Squad(int id, 
		Tactic* defaultTactic, 
		const std::string& name, 
		unsigned int priority, 
		unsigned int activePriority);

	void computeActions();
	void assist();
	void 


	UnitSetup& getSetup();
	UnitCollection& getUnits();
	int getID() const;
	int getPriority() const;
	int getActivePriority() const;
	const std::string& getName() const;


	/** Add a member to the squad */
	void addMember(BaseAgent* agent);


	/** Push a tactic to be used, until popped */
	void pushTactic(Tactic* tactic);

	/** Pop the topmost tactic from the stack (the default one (bottom) will never be popped) */
	void popTactic();

	/** Set the bottom most tactic, the tactic the squad will use unless having temporary tactics pushed */
	void setDefaultTactic(Tactic* tactic);


	/** See if we have all units required */
	bool isActive() const;

	/** See if all units in this squad is required */
	bool isRequired() const;

	/** See if we need a unit of the given type */
	bool needUnit(const BWAPI::UnitType& type) const;
private:
	unsigned int m_id;
	unsigned int m_priority;
	unsigned int m_activePriority;
	bool m_required;
	std::string m_name;

	UnitSetup m_setup;
	UnitCollection m_units;
	std::vector<Tactic*> m_tacticStack;

	BWAPI::TilePosition m_goal;
	int m_goalSetFrame;
	int m_goalArrivedFrame;
};

#endif