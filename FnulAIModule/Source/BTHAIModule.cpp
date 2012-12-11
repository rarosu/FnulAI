#include <BTHAIModule.h>

#include <Shlwapi.h>

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
bool leader = false;


DWORD WINAPI AnalyzeThread()
{
	// TODO: Why doesn't this work for every level, if we do not already have analyzed data for it?
	BWTA::analyze();
	
	// TODO: Is there any difference between these? Are they even USED?
	//		 Note: They are extern global variables. Could be used anywhere...
	analyzed = true;
	analysis_just_finished = true;

	return 0;
}


BTHAIModule::BTHAIModule()
	: m_speed(0)
	, m_profile(false)
	, m_logger("bwapi-data\\AI\\FnulAILog.txt")
{
	m_logger.printfln("Loading the FnulAI module");


	// Needed for BWAPI to work
	Broodwar->enableFlag(Flag::UserInput);

	//Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the game speed
	SetSpeed(m_speed);

	// Set the initial debug mode
	m_loop.setDebugMode(1);

	// Be polite!
	Broodwar->sendText("glhf");
}

BTHAIModule::~BTHAIModule()
{
	Pathfinder::Instance().stop();
	Profiler::Instance().dumpToFile();

	// Be polite!
	Broodwar->sendText("gg");

	m_logger.printfln("Releasing the FnulAI module");
}

void BTHAIModule::onStart() 
{
	Profiler::Instance().start("OnInit");
	
	// Analyze map using BWTA (use separate thread)
	BWTA::readMap();
	analyzed = false;
	analysis_just_finished = false;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	//AnalyzeThread();


	// If replay, list all players and their races
	if (Broodwar->isReplay()) 
	{
		Broodwar->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
	}
	
    // Add the units we have from start to agent manager
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++) 
	{
		AgentManager::Instance().addAgent(*i);
	}

	Profiler::Instance().end("OnInit");
}

void BTHAIModule::onEnd(bool isWinner) 
{
	PrintStatistics(isWinner);
}

void BTHAIModule::onFrame() 
{
	Profiler::Instance().start("OnFrame");


	if (!Broodwar->isInGame()) 
	{
		//Not in game. Do nothing.
		return;
	}

	if (Broodwar->isReplay()) 
	{
		//Replay. Do nothing.
		return;
	}
	
	// Update the AI per frame logic
	m_loop.computeActions();
	m_loop.show_debug();

	// Display the name of this bot on screen
	Config::Instance().displayBotName();


	Profiler::Instance().end("OnFrame");

	// Display profiling information
	if (m_profile) 
		Profiler::Instance().showAll();
}

void BTHAIModule::onSendText(std::string text) 
{
	if (text=="/a") 
	{
		Commander::Instance().forceAttack();
	}
	else if(text=="/p") 
	{
		m_profile = !m_profile;
	}
	else if(text=="/d1") 
	{
		m_loop.setDebugMode(1);
	}
	else if(text=="/d2") 
	{
		m_loop.setDebugMode(2);
	}
	else if(text=="/d3") 
	{
		m_loop.setDebugMode(3);
	}
	else if(text=="/off") 
	{
		m_loop.setDebugMode(0);
	}
	else if(text=="/d0") 
	{
		m_loop.setDebugMode(0);
	}
	else if (text.substr(0, 2)=="sq") 
	{
		int id = atoi(&text[2]);
		Squad* squad = Commander::Instance().getSquad(id);
		if (squad != NULL) 
		{
			squad->printFullInfo();
		}
	}
	else if (text=="+") 
	{
		SetSpeed(m_speed - 4);

		Broodwar->printf("Speed increased to %d", m_speed);
	}
	else if (text=="++") 
	{
		SetSpeed(0);

		Broodwar->printf("Speed increased to %d", m_speed);
	}
	else if (text=="-") 
	{
		SetSpeed(m_speed + 4);

		Broodwar->printf("Speed decreased to %d", m_speed);
	}
	else if (text=="--") 
	{
		SetSpeed(24);

		Broodwar->printf("Speed decreased to %d", m_speed);
	}
	else if (text=="i") 
	{
		PrintSelectedUnitInfo();
	}
	else 
	{
		//Broodwar->printf("You typed '%s'!",text.c_str());
		Broodwar->sendText(text.c_str());
	}
}

void BTHAIModule::onReceiveText(BWAPI::Player* player, std::string text) 
{
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void BTHAIModule::onPlayerLeft(BWAPI::Player* player) 
{
	Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void BTHAIModule::onNukeDetect(BWAPI::Position target) 
{
	// TODO: React on this! Make all units move outside of the blast radius?
	//		 Notify the commander and let it make a decision?
	if (target != Positions::Unknown) 
	{
		TilePosition t = TilePosition(target);
		Broodwar->printf("Nuclear Launch Detected at (%d,%d)",t.x(),t.y());
	}
	else
	{
		Broodwar->printf("Nuclear Launch Detected");
	}
}

void BTHAIModule::onUnitDiscover(BWAPI::Unit* unit) 
{
	// TODO: Notify the commander about new intel?
}

void BTHAIModule::onUnitEvade(BWAPI::Unit* unit) 
{
	// TODO: Determine what the hell this means...
}

void BTHAIModule::onUnitShow(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) 
		return;

	// We located a new unit. If it is an enemy, add it to our spotted units.
	if (unit->getPlayer()->getID() != Broodwar->self()->getID()) 
	{
		if (!unit->getPlayer()->isNeutral() && !unit->getPlayer()->isAlly(Broodwar->self()))
		{
			ExplorationManager::Instance().addSpottedUnit(unit);
		}
	}
}

void BTHAIModule::onUnitHide(BWAPI::Unit* unit) 
{
	// TODO: Determine what this is too...
}

void BTHAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) 
		return;

	m_loop.addUnit(unit);
}

void BTHAIModule::onUnitDestroy(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) 
		return;

	m_loop.unitDestroyed(unit);
}

void BTHAIModule::onUnitMorph(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) 
		return;

	// TODO: Look into this. Why not call morphUnit for archons?
	if (BuildPlanner::isZerg())
	{
		m_loop.morphUnit(unit);
	}
	else
	{
		m_loop.addUnit(unit);
	}
}

void BTHAIModule::onUnitRenegade(BWAPI::Unit* unit) 
{
	// TODO: wut?
}

void BTHAIModule::onSaveGame(std::string gameName) 
{
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}



void BTHAIModule::PrintSelectedUnitInfo()
{
	set<Unit*> units = Broodwar->getSelectedUnits();
	if ((int)units.size() > 0) 
	{
		int unitID = (*units.begin())->getID();
		BaseAgent* agent = AgentManager::Instance().getAgent(unitID);
		if (agent != NULL) 
		{
			agent->printInfo();
		}
		else 
		{
			Unit* mUnit = (*units.begin());
			if (mUnit->getType().isNeutral())
			{
				//Neutral unit. Check distance to base.
				BaseAgent* agent = AgentManager::Instance().getAgent(UnitTypes::Terran_Command_Center);
				double dist = agent->getUnit()->getDistance(mUnit);
				Broodwar->printf("Distance to base: %d", (int)dist);
			}
		}
	}
}

void BTHAIModule::SetSpeed(int speed)
{
	if (speed < 0)
		speed = 0;
	if (speed > 24)
		speed = 24;
	
	m_speed = speed;
	Broodwar->setLocalSpeed(m_speed);
}

void BTHAIModule::PrintStatistics(bool isWinner)
{
	std::ofstream file("bwapi-data\\AI\\FnulAIStatistics.txt", std::ios::out | std::ios::trunc);

	if (file.is_open())
	{
		file << "Game Won: " << isWinner << std::endl;
		file << std::endl;
		file << "Self Race: " << Broodwar->self()->getRace().getID() << std::endl;
		file << "Self Minerals Mined: " << Broodwar->self()->cumulativeMinerals() << std::endl;
		file << "Self Gas Mined: " << Broodwar->self()->cumulativeGas() << std::endl;
		file << std::endl;
		file << "Enemy Race: " << Broodwar->enemy()->getRace().getID() << std::endl;
		file << "Enemy Minerals Mined: " << Broodwar->enemy()->cumulativeMinerals() << std::endl;
		file << "Enemy Gas Mined: " << Broodwar->enemy()->cumulativeGas() << std::endl;
		file << std::endl;
		
		int nTotalUserKilled = 0;
		int nTotalEnemyKilled = 0;
		int nTotalUserScore = 0;
		int nTotalEnemyScore = 0;
		for (std::set<UnitType>::const_iterator unitType = UnitTypes::allUnitTypes().begin(); unitType != UnitTypes::allUnitTypes().end(); unitType++)
		{
			nTotalUserKilled += Broodwar->self()->killedUnitCount(*unitType);
			nTotalEnemyKilled += Broodwar->enemy()->killedUnitCount(*unitType);
			nTotalUserScore += nTotalUserKilled * (*unitType).destroyScore();
			nTotalEnemyScore += nTotalEnemyKilled * (*unitType).destroyScore();
		}

		int nTotalScore = nTotalUserScore - nTotalEnemyScore;

		file << "Self Kill Count: " << nTotalUserKilled << std::endl;
		file << "Self Kill Score: " << nTotalUserScore << std::endl;
		file << "Self Dead Score: " << Commander::Instance().ownDeadScore << std::endl;
		file << std::endl;
		file << "Enemy Kill Count: " << nTotalEnemyKilled << std::endl;
		file << "Enemy Kill Score: " << nTotalEnemyScore << std::endl;
		file << "Enemy Dead Score: " << Commander::Instance().enemyDeadScore << std::endl;
		file << std::endl;
		file << "Total Score: " << nTotalScore << std::endl;
	}

	file.close();
}




bool BTHAITournamentModule::onAction(int actionType, void *parameter)
{
	switch ( actionType )
	{
		case Tournament::SendText:
		case Tournament::Printf:
		// Call our bad word filter and allow the AI module to send text
		return true;

		case Tournament::EnableFlag:
			switch ( *(int*)parameter )
			{
			case Flag::CompleteMapInformation:
			case Flag::UserInput:
				// Disallow these two flags
				return false;
			}

			// Allow other flags if we add more that don't affect gameplay specifically
			return true;

		case Tournament::LeaveGame:
		case Tournament::PauseGame:
		case Tournament::RestartGame:
		case Tournament::ResumeGame:
		case Tournament::SetFrameSkip:
		case Tournament::SetGUI:
		case Tournament::SetLocalSpeed:
		case Tournament::SetMap:
		return false; // Disallow these actions

		case Tournament::ChangeRace:
		case Tournament::SetLatCom:
		case Tournament::SetTextSize:
		return true; // Allow these actions

		case Tournament::SetCommandOptimizationLevel:
		return *(int*)parameter > MINIMUM_COMMAND_OPTIMIZATION; // Set a minimum command optimization level 
																// to reduce APM with no action loss
		default:
		break;
	}

	return true;
}

void BTHAITournamentModule::onFirstAdvertisement()
{
	leader = true;
	Broodwar->sendText("Welcome to " TOURNAMENT_NAME "!");
	Broodwar->sendText("Brought to you by " SPONSORS ".");
}



