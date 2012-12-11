#pragma once

#include <Commander\Commander.h>
#include <MainAgents\AgentFactory.h>
#include <Managers\AgentManager.h>
#include <Managers\BuildPlanner.h>
#include <Managers\CoverMap.h>
#include <Managers\ExplorationManager.h>
#include <Managers\ResourceManager.h>
#include <Managers\UpgradesPlanner.h>
#include <PotentialFields\PFManager.h>
#include <Utils\Pathfinder.h>
#include <Utils\Profiler.h>
#include <Utils\Config.h>
#include <Utils\Logger.h>
#include <Utils\ScopedPointer.h>
#include <AIloop.h>

#include <BWTA.h>
#include <windows.h>
#include <time.h>
#include <BWTAExtern.h>


#define TOURNAMENT_NAME "AIIDE 2011"
#define SPONSORS "the BWAPI Project Team"
#define MINIMUM_COMMAND_OPTIMIZATION 1


// TODO: Look into if we need to use this...
class BTHAITournamentModule : public BWAPI::TournamentModule
{
	virtual bool onAction(int actionType, void *parameter = NULL);
	virtual void onFirstAdvertisement();
};


/** This class contains the main game loop and all events that is broadcasted from the Starcraft engine
 * using BWAPI. See the BWAPI documentation for more info. 
 *
 * Author: Contained in BWAPI 3.0.3
 * Modified: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BTHAIModule : public BWAPI::AIModule
{
private:
	int m_speed;
	bool m_profile;

	Logger m_logger;
	Config m_config;
	Profiler m_profiler;

	Commander m_commander;
	AgentFactory m_agentFactory;
	AgentManager m_agentManager;
	BuildPlanner m_buildPlanner;
	CoverMap m_coverMap;
	ExplorationManager m_explorationManager;
	ResourceManager m_resourceManager;
	UpgradesPlanner m_upgradesPlanner;
	PFManager m_pfManager;
	Pathfinder m_pathfinder;

	AIloop m_loop;


	void PrintSelectedUnitInfo();
	void SetSpeed(int speed);
	void PrintStatistics(bool isWinner);
public:
	BTHAIModule();
	~BTHAIModule();

	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
};
