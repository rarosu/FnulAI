#ifndef __UPGRADESPLANNER_H__
#define __UPGRADESPLANNER_H__

#include <BWAPI.h>
#include <MainAgents\BaseAgent.h>
#include <r2-singleton.hpp>

using namespace BWAPI;
using namespace std;

/** UpgradesPlanner contains which updates/techs to be research and in
 * which order.
 *
 * The UpgradesPlanner is implemented as a singleton class. Each class that needs to
 * access UpgradesPlanner can request an instance, and all classes shares the same UpgradesPlanner instance.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class UpgradesPlanner : public r2::Singleton<UpgradesPlanner> {

private:
	vector<UpgradeType> upgradesP1;
	vector<UpgradeType> upgradesP2;
	vector<UpgradeType> upgradesP3;
	vector<TechType> techsP1;
	vector<TechType> techsP2;
	vector<TechType> techsP3;

	bool canUpgrade(UpgradeType type, Unit* unit);
	bool canResearch(TechType type, Unit* unit);
public:
	UpgradesPlanner();
	~UpgradesPlanner();


	/** Checks if there is an upgrade the specified agent need to upgrade/research. */
	bool checkUpgrade(BaseAgent* agent);
};

#endif
