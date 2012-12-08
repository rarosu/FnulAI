#pragma once

#include <Utils\Logger.h>
#include <Managers\AgentManager.h>
#include <Statistics\Statistics.h>
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

// TODO: Move to the source file?
DWORD WINAPI AnalyzeThread();

/** This class contains the main game loop and all events that is broadcasted from the Starcraft engine
 * using BWAPI. See the BWAPI documentation for more info. 
 *
 * Author: Contained in BWAPI 3.0.3
 * Modified: Johan Hagelback (johan.hagelback@gmail.com)
 */
class BTHAIModule : public BWAPI::AIModule
{
private:
	int speed;
	Statistics* statistics;
	AIloop* loop;
	Logger m_logger;
	bool running;
	bool profile;


	void gameStopped();

public:
	// TODO: Add constructor & destructor and implement RAII
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
