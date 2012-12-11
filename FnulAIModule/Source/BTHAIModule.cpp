#include <BTHAIModule.h>

#include <Shlwapi.h>

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
bool leader = false;

BTHAIModule::BTHAIModule()
	: m_logger("bwapi-data\\AI\\FnulAILog.txt")
{
	m_logger.printfln("Loading the FnulAI module");
}

BTHAIModule::~BTHAIModule()
{
	m_logger.printfln("Releasing the FnulAI module");
}

void BTHAIModule::onStart() 
{
	

	Profiler::Instance().start("OnInit");

	//Needed for BWAPI to work
	Broodwar->enableFlag(Flag::UserInput);

	//Set max speed
	speed = 8; //10
	Broodwar->setLocalSpeed(speed);

	//Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	
	//Analyze map using BWTA (use separate thread)
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL); //Threaded version
	//AnalyzeThread();

	profile = false;


	// TODO: Need a better memory management model
	loop = new AIloop();
	loop->setDebugMode(1);

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
	
    //Add the units we have from start to agent manager
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++) 
	{
		AgentManager::Instance().addAgent(*i);
	}

	running = true;

	Profiler::Instance().end("OnInit");
}

void BTHAIModule::gameStopped()
{
	//statistics->WriteStatisticsFile(isWinner);
	//Pathfinder::Instance().stop();
	//delete(statistics);
	//Profiler::Instance().dumpToFile();
	running = false;
}

void BTHAIModule::onEnd(bool isWinner) 
{
	gameStopped();
}

void BTHAIModule::onFrame() 
{
	Profiler::Instance().start("OnFrame");

	if (!running) 
	{
		//Game over. Do nothing.
		return;
	}

	if (!Broodwar->isInGame()) 
	{
		//Not in game. Do nothing.
		gameStopped();
		return;
	}

	if (Broodwar->isReplay()) 
	{
		//Replay. Do nothing.
		return;
	}
	
	// Update the AI per frame logic
	loop->computeActions();
	loop->show_debug();

	// Display the name of this bot on screen
	Config::Instance().displayBotName();

	Profiler::Instance().end("OnFrame");

	// Display profiling information
	if (profile) Profiler::Instance().showAll();
}

void BTHAIModule::onSendText(std::string text) 
{
	if (text=="/a") 
	{
		Commander::Instance().forceAttack();
	}
	else if(text=="/p") 
	{
		profile = !profile;
	}
	else if(text=="/d1") 
	{
		loop->setDebugMode(1);
	}
	else if(text=="/d2") 
	{
		loop->setDebugMode(2);
	}
	else if(text=="/d3") 
	{
		loop->setDebugMode(3);
	}
	else if(text=="/off") 
	{
		loop->setDebugMode(0);
	}
	else if(text=="/d0") 
	{
		loop->setDebugMode(0);
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
		speed -= 4;
		if (speed < 0) 
		{
			speed = 0;
		}
		Broodwar->printf("Speed increased to %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="++") 
	{
		speed = 0;
		Broodwar->printf("Speed increased to %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="-") 
	{
		// TODO: Impose limit?
		speed += 4;
		Broodwar->printf("Speed decreased to %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="--") 
	{
		speed = 24;
		Broodwar->printf("Speed decreased to %d", speed);
		Broodwar->setLocalSpeed(speed);
	}
	else if (text=="i") 
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
	if (player->getID() == Broodwar->self()->getID())
	{
		gameStopped();
	}

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
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

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
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	loop->addUnit(unit);
}

void BTHAIModule::onUnitDestroy(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	loop->unitDestroyed(unit);
}

void BTHAIModule::onUnitMorph(BWAPI::Unit* unit) 
{
	if (Broodwar->isReplay() || Broodwar->getFrameCount() <= 1) return;

	// TODO: Look into this. Why not call morphUnit for archons?
	if (BuildPlanner::isZerg())
	{
		loop->morphUnit(unit);
	}
	else
	{
		loop->addUnit(unit);
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
