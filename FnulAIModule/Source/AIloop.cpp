#include <AIloop.h>
#include <Managers\CoverMap.h>
#include <Utils\Profiler.h>
#include <Managers\ResourceManager.h>

AIloop::AIloop()
{
	
}

AIloop::~AIloop()
{
	
}

void AIloop::setDebugMode(int mode)
{
	debug_mode = mode;
}

void AIloop::computeActions()
{
	//First, do some checks to see if it is time to resign
	if (AgentManager::Instance().noMiningWorkers() == 0 && Broodwar->self()->minerals() <= 50)
	{
		Broodwar->printf("No workers left. Bailing out.");
		Broodwar->leaveGame();
		return;
	}

	if (AgentManager::Instance().countNoBases() == 0 && Broodwar->getFrameCount() > 500)
	{
		Broodwar->printf("No bases left. Bailing out.");
		Broodwar->leaveGame();
		return;
	}

	// TODO: If the singletons are changed, this needs to be changed too.
	AgentManager::Instance().computeActions();
	BuildPlanner::Instance().computeActions();
	Commander::Instance().computeActions();
	ExplorationManager::Instance().computeActions();
}

void AIloop::addUnit(Unit* unit)
{
	AgentManager::Instance().addAgent(unit);

	// TODO: wut? Buildings are not in the build order?
	//Remove from buildorder if this is a building
	if (unit->getType().isBuilding())
	{
		BuildPlanner::Instance().unlock(unit->getType());
	}
}

void AIloop::morphUnit(Unit* unit)
{
	// TODO: Look this up. Morph drone? Always? What about mutalisks?
	AgentManager::Instance().morphDrone(unit);
	BuildPlanner::Instance().unlock(unit->getType());
}

void AIloop::unitDestroyed(Unit* unit)
{
	// One of our units were destroyed...
	if (unit->getPlayer()->getID() == Broodwar->self()->getID())
	{
		AgentManager::Instance().removeAgent(unit);
		if (unit->getType().isBuilding())
		{
			BuildPlanner::Instance().buildingDestroyed(unit);
		}

		// NOTE: Agents are not immediately removed, only removed at AgentManager::cleanup

		// TODO: Assist workers that have been destroyed? They have just been removed.
		// WARNING: Potential danger
		//Assist workers under attack
		if (unit->getType().isWorker())
		{
			Commander::Instance().assistWorker(AgentManager::Instance().getAgent(unit->getID()));
		}

		// WARNING: Potential danger
		//Update dead score
		if (unit->getType().canMove())
		{
			Commander::Instance().ownDeadScore += unit->getType().destroyScore();
		}

		// TODO: Look this one up.
		AgentManager::Instance().cleanup();
	}

	// TODO: Will allies count into the neutral category? Otherwise, we count allied death as a good thing...
	// One of our enemies' or allies' units were destroyed
	if (unit->getPlayer()->getID() != Broodwar->self()->getID() && !unit->getPlayer()->isNeutral())
	{
		ExplorationManager::Instance().unitDestroyed(unit);
		Commander::Instance().enemyDeadScore += unit->getType().destroyScore();
	}
}

void AIloop::show_debug()
{
	if (debug_mode > 0)
	{
		// Show the goals of our units
		vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			if (agents.at(i)->isBuilding()) agents.at(i)->debug_showGoal();
			if (!agents.at(i)->isBuilding() && debug_mode >= 2) agents.at(i)->debug_showGoal();
		}

		// Type manager information
		BuildPlanner::Instance().printInfo();
		ExplorationManager::Instance().printInfo();
		Commander::Instance().printInfo();
		
		if (debug_mode >= 3) CoverMap::Instance().debug();
		if (debug_mode >= 2) ResourceManager::Instance().printInfo();

		Commander::Instance().debug_showGoal();

		// Draw information from the terrain analysis
		if (debug_mode >= 1)
		{
			drawTerrainData();
		}
	}
}

void AIloop::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
	{
		TilePosition p=(*i)->getTilePosition();
		Position c=(*i)->getPosition();

		//Draw a progress bar at each resource
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
		{
			if ((*j)->getResources() > 0)
			{
				
				int total = (*j)->getInitialResources();
				int done = (*j)->getResources();

				int w = 60;
				int h = 64;

				//Start 
				Position s = Position((*j)->getPosition().x() - w/2 + 2, (*j)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),e.x(),e.y(),Colors::Orange,false);
				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),p.x(),p.y(),Colors::Orange,true);
			}
		}
	}

	if (debug_mode >= 2)
	{
		//we will iterate through all the regions and draw the polygon outline of it in white.
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			BWTA::Polygon p=(*r)->getPolygon();
			for(int j=0;j<(int)p.size();j++)
			{
				Position point1=p[j];
				Position point2=p[(j+1) % p.size()];
				Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Brown);
			}
		}

		//we will visualize the chokepoints with yellow lines
		for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
		{
			for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
			{
				Position point1=(*c)->getSides().first;
				Position point2=(*c)->getSides().second;
				Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Yellow);
			}
		}
	}

	//locate zerg eggs and draw progress bars
	if (BuildPlanner::isZerg())
	{
		for(std::set<Unit*>::const_iterator u = Broodwar->self()->getUnits().begin(); u != Broodwar->self()->getUnits().end(); u++)
		{
			if ((*u)->getType().getID() == UnitTypes::Zerg_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Lurker_Egg.getID() || (*u)->getType().getID() == UnitTypes::Zerg_Cocoon.getID())
			{
				int total = (*u)->getBuildType().buildTime();
				int done = total - (*u)->getRemainingBuildTime();
				
				int w = (*u)->getType().tileWidth() * 32;
				int h = (*u)->getType().tileHeight() * 32;

				//Start 
				Position s = Position((*u)->getPosition().x() - w/2, (*u)->getPosition().y() - 4);
				//End
				Position e = Position(s.x() + w, s.y() + 8);
				//Progress
				int prg = (int)((double)done / (double)total * w);
				Position p = Position(s.x() + prg, s.y() +  8);

				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),e.x(),e.y(),Colors::Blue,false);
				Broodwar->drawBox(CoordinateType::Map,s.x(),s.y(),p.x(),p.y(),Colors::Blue,true);
			}
		}
	}
}
