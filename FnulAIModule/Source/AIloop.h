#ifndef __AILOOP_H__
#define __AILOOP_H__

#include <MainAgents\BaseAgent.h>
#include <Managers\AgentManager.h>
#include <Managers\ExplorationManager.h>
#include <Commander\Commander.h>
#include <Managers\BuildPlanner.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

/** This class handles the main AI loop that is executed each frame. It is 
 * separated from the BTHAIModule::onFrame() to make it possible to use
 * multi-threading.
 *
 * Author: Johan Hagelback (johan.hagelback@gmail.com)
 */
class AIloop {

private:
	int debug_mode;

	void drawTerrainData();

public:
	/** Constructor */
	AIloop();

	/** Destructor */
	~AIloop();

	/** Call this each AI frame. */
	void computeActions();
	
	/** Sets the debug level for the AI (0-3). 0 = no debug, 3 = full debug. */
	void setDebugMode(int mode);

	/** Show debug info. */
	void show_debug();

	/** Called when a new unit is added to the game. */
	void addUnit(Unit* unit);

	/** Called when a unit is destroyed in the game. */
	void unitDestroyed(Unit* unit);

	/** Called when a unit is morphed in in the game. */
	void morphUnit(Unit* unit);
};

#endif
