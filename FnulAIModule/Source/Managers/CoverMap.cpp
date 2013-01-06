#include <Managers\CoverMap.h>
#include <Managers\AgentManager.h>
#include <Managers\ExplorationManager.h>
#include <MainAgents\BaseAgent.h>
#include <Managers\BuildPlanner.h>
#include <Commander\Commander.h>
#include <Utils\Profiler.h>

CoverMap::CoverMap()
{
	w = Broodwar->mapWidth();
	h = Broodwar->mapHeight();
	range = 30;

	Unit* worker = findWorker();

	cover_map = new int*[w];
	for(int i = 0 ; i < w ; i++)
	{
		cover_map[i] = new int[h];

		//Fill from static map and Region connectability
		for (int j = 0; j < h; j++)
		{
			int ok = BUILDABLE;
			if (!Broodwar->isBuildable(i, j))
			{
				ok = BLOCKED;
			}

			cover_map[i][j] = ok;
		}
	}

	//Fill from current agents
	vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isBuilding())
		{
			Corners c = getCorners(agent->getUnit());
			fill(c);
		}
	}

	//Fill from minerals
	for(set<Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		Corners c;
		c.x1 = (*m)->getTilePosition().x() - 1;
		c.y1 = (*m)->getTilePosition().y() - 1;
		c.x2 = (*m)->getTilePosition().x() + 2;
		c.y2 = (*m)->getTilePosition().y() + 1;
		fill(c);

		cover_map[c.x1+2][c.y1+2] = MINERAL;
	}

	//Fill from gas
	for(set<Unit*>::iterator m = Broodwar->getGeysers().begin(); m != Broodwar->getGeysers().end(); m++)
	{
		Corners c;
		c.x1 = (*m)->getTilePosition().x() - 2;
		c.y1 = (*m)->getTilePosition().y() - 2;
		c.x2 = (*m)->getTilePosition().x() + 5;
		c.y2 = (*m)->getTilePosition().y() + 3;
		fill(c);

		cover_map[c.x1+2][c.y1+2] = GAS;
	}

	//Fill from narrow chokepoints
	if (analyzed)
	{
		for(set<Region*>::const_iterator i=getRegions().begin();i!=getRegions().end();i++)
		{
			for(set<Chokepoint*>::const_iterator c=(*i)->getChokepoints().begin();c!=(*i)->getChokepoints().end();c++)
			{
				if ((*c)->getWidth() <= 4 * 32)
				{
					TilePosition center = TilePosition((*c)->getCenter());
					Corners c;
					c.x1 = center.x() - 1;
					c.x2 = center.x() + 1;
					c.y1 = center.y() - 1;
					c.y2 = center.y() + 1;
					fill(c);
				}
			}
		}
	}

	mapData = MapDataReader();
	mapData.readMap();
}

CoverMap::~CoverMap()
{
	for(int i = 0 ; i < w ; i++)
	{
		delete[] cover_map[i];
	}

	delete[] cover_map;
}


Unit* CoverMap::findWorker()
{
	BaseAgent* worker = AgentManager::Instance().getAgent(Broodwar->self()->getRace().getWorker());
	if (worker != NULL)
	{
		return worker->getUnit();
	}
	return NULL;
}

bool CoverMap::positionFree(TilePosition pos)
{
	if (cover_map[pos.x()][pos.y()] == BUILDABLE)
	{
		return true;
	}
	return false;
}

void CoverMap::blockPosition(TilePosition buildSpot)
{
	if (buildSpot.x() == -1)
	{
		//Error check
		return;
	}
	cover_map[buildSpot.x()][buildSpot.y()] = BLOCKED;
}

bool CoverMap::canBuild(UnitType toBuild, TilePosition buildSpot)
{
	Corners c = getCorners(toBuild, buildSpot);

	//Step 1: Check covermap.
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] != BUILDABLE)
				{
					//Cant build here.
					return false;
				}
			}
			else
			{
				//Out of bounds
				return false;
			}
		}
	}

	//Step 2: Check if path is available
	if (!ExplorationManager::canReach(Broodwar->self()->getStartLocation(), buildSpot))
	{
		return false;
	}

	//Step 3: Check canBuild
	Unit* worker = findWorker();
	if (worker == NULL)
	{
		//No worker available
		return false;
	}

	//Step 4: Check any units on tile
	if (AgentManager::Instance().unitsInArea(buildSpot, toBuild.tileWidth(), toBuild.tileHeight(), worker->getID()))
	{
		return false;
	}

	//Step 5: If Protoss, check PSI coverage
	if (BuildPlanner::isProtoss())
	{
		if (toBuild.requiresPsi())
		{
			if (!Broodwar->hasPower(buildSpot, toBuild.tileWidth(), toBuild.tileHeight()))
			{
				return false;
			}
		}
	}

	//Step 6: If Zerg, check creep
	if (BuildPlanner::isZerg())
	{
		if (UnitAgent::isOfType(toBuild, UnitTypes::Zerg_Hatchery))
		{
			//Do not build if we have creep (to spread creep out)
			if (Broodwar->hasCreep(buildSpot))
			{
				return false;
			}
		}
		else if (toBuild.requiresCreep())
		{
			if (!Broodwar->hasCreep(buildSpot))
			{
				return false;
			}
		}
	}

	//Step 7: If detector, check if spot is already covered by a detector
	/*if (toBuild.isDetector())
	{
		if (!suitableForDetector(buildSpot))
		{
			return false;
		}
	}*/

	//All passed. It is possible to build here.
	return true;
}

TilePosition CoverMap::findBuildSpot(UnitType toBuild)
{
	//Refinery
	if (toBuild.isRefinery())
	{
		//Use refinery method
		return findRefineryBuildSpot(toBuild, Broodwar->self()->getStartLocation());
	}

	//If we find unpowered buildings, build a Pylon there
	if (BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Pylon))
	{
		vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* agent = agents.at(i);
			if (agent->isAlive())
			{
				Unit* cUnit = agent->getUnit();
				if (cUnit->isUnpowered())
				{
					//Broodwar->printf("Build pylon at unpowered building %s", cUnit->getType().getName().c_str());
					TilePosition spot = findBuildSpot(toBuild, cUnit->getTilePosition());
					return spot;
				}
			}
		}
	}

	if (BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Pylon))
	{
		if (AgentManager::Instance().countNoUnits(UnitTypes::Protoss_Pylon) > 0)
		{
			TilePosition cp = Commander::Instance().findChokePoint();
			if (cp.x() != -1)
			{
				if (!Broodwar->hasPower(cp, UnitTypes::Protoss_Cybernetics_Core))
				{
					if (AgentManager::Instance().noInProduction(UnitTypes::Protoss_Pylon) == 0)
					{
						TilePosition spot = findBuildSpot(toBuild, cp);
						return spot;
					}
				}
			}
		}
	}

	//Build near chokepoints: Bunker, Photon Cannon, Creep Colony
	if (BaseAgent::isOfType(toBuild, UnitTypes::Terran_Bunker) || BaseAgent::isOfType(toBuild, UnitTypes::Protoss_Photon_Cannon) || BaseAgent::isOfType(toBuild, UnitTypes::Zerg_Creep_Colony))
	{
		TilePosition cp = Commander::Instance().findChokePoint();
		if (cp.x() != -1)
		{
			TilePosition spot = findBuildSpot(toBuild, cp);
			return spot;
		}
	}

	//Base buildings.
	if (toBuild.isResourceDepot())
	{
		TilePosition start = ExplorationManager::Instance().searchExpansionSite();
		if (start.x() != -1)
		{
			TilePosition spot = findBuildSpot(toBuild, start);
			return spot;
		}
		else
		{
			//No expansion site found.
			return TilePosition(-1, -1);
		}
	}

	//General building. Search for spot around bases
	vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
	for (int i = (int)agents.size() - 1; i >= 0; i--)
	{
		BaseAgent* agent = agents.at(i);
		if (agent->isAlive() && agent->getUnitType().isResourceDepot() && !baseUnderConstruction(agent))
		{
			TilePosition start = agent->getUnit()->getTilePosition();
			TilePosition bSpot = findBuildSpot(toBuild, start);
			if (bSpot.x() != -1)
			{
				//Spot found, return it.
				return bSpot;
			}
		}
	}

	return TilePosition(-1, -1);
}

TilePosition CoverMap::findClosestBuildSpot(UnitType toBuild, TilePosition start)
{
	//Check start pos
	if (canBuildAt(toBuild, start)) return start;

	//Search outwards
	int cDiff = 1;
	TilePosition bestSpot = TilePosition(-1, -1);
	double bestDistance = 1000000.0;
	while (true) 
	{
		//Top
		TilePosition s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		TilePosition e = TilePosition(start.x() + cDiff, start.y() - cDiff);
		TilePosition spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			double distance = mapData.getDistance(start, spot);
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestSpot = spot;
			}
		}

		//Bottom
		s = TilePosition(start.x() - cDiff, start.y() + cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			double distance = mapData.getDistance(start, spot);
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestSpot = spot;
			}
		}

		//Left
		s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		e = TilePosition(start.x() - cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			double distance = mapData.getDistance(start, spot);
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestSpot = spot;
			}
		}

		//Right
		s = TilePosition(start.x() + cDiff, start.y() - cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			double distance = mapData.getDistance(start, spot);
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestSpot = spot;
			}
		}

		cDiff++;
		if (cDiff > range)
			break;
	}
	
	return bestSpot;
}

bool CoverMap::baseUnderConstruction(BaseAgent* base)
{
	if (BuildPlanner::isTerran())
	{
		return base->getUnit()->isBeingConstructed();
	}
	if (BuildPlanner::isProtoss())
	{
		return base->getUnit()->isBeingConstructed();
	}
	if (BuildPlanner::isZerg())
	{
		if (base->isOfType(UnitTypes::Zerg_Hatchery))
		{
			return base->getUnit()->isBeingConstructed();
		}
	}
	return false;
}

TilePosition CoverMap::findSpotAtSide(UnitType toBuild, TilePosition start, TilePosition end)
{
	int dX = end.x() - start.x();
	if (dX != 0) dX = 1;
	int dY = end.y() - start.y();
	if (dY != 0) dY = 1;

	TilePosition cPos = start;
	bool done = false;
	while (!done) 
	{
		if (canBuildAt(toBuild, cPos)) return cPos;
		int cX = cPos.x() + dX;
		int cY = cPos.y() + dY;
		cPos = TilePosition(cX, cY);
		if (cPos.x() == end.x() && cPos.y() == end.y()) done = true;
	}

	return TilePosition(-1, -1);
}

bool CoverMap::canBuildAt(UnitType toBuild, TilePosition pos)
{
	int maxW = w - toBuild.tileWidth() - 1;
	int maxH = h - toBuild.tileHeight() - 1;

	//Out of bounds check
	if (pos.x() >= 0 && pos.x() < maxW && pos.y() >= 0 && pos.y() < maxH)
	{
		if (canBuild(toBuild, pos))
		{
			return true;
		}
	}
	return false;
}

TilePosition CoverMap::findBuildSpot(UnitType toBuild, TilePosition start)
{
	//Check start pos
	if (canBuildAt(toBuild, start)) return start;

	//Search outwards
	bool found = false;
	int cDiff = 1;
	TilePosition spot = TilePosition(-1, -1);
	while (!found) 
	{
		//Top
		TilePosition s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		TilePosition e = TilePosition(start.x() + cDiff, start.y() - cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Bottom
		s = TilePosition(start.x() - cDiff, start.y() + cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Left
		s = TilePosition(start.x() - cDiff, start.y() - cDiff);
		e = TilePosition(start.x() - cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		//Right
		s = TilePosition(start.x() + cDiff, start.y() - cDiff);
		e = TilePosition(start.x() + cDiff, start.y() + cDiff);
		spot = findSpotAtSide(toBuild, s, e);
		if (spot.x() != -1 && spot.y() != -1)
		{
			found = true;
			break;
		}

		cDiff++;
		if (cDiff > range) found = true;
	}
	
	return spot;
}

void CoverMap::addConstructedBuilding(Unit* unit)
{
	if (unit->getType().isAddon())
	{
		//Addons are handled by their main buildings
		return;
	}

	Corners c = getCorners(unit);
	fill(c);
}

void CoverMap::buildingDestroyed(Unit* unit)
{
	if (unit->getType().isAddon())
	{
		//Addons are handled by their main buildings
		return;
	}

	Corners c = getCorners(unit);
	clear(c);
}

void CoverMap::fill(Corners c)
{
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BUILDABLE)
				{
					cover_map[x][y] = BLOCKED;
				}
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BLOCKED;
				}
			}
		}
	}
}

void CoverMap::fillTemp(UnitType toBuild, TilePosition buildSpot)
{
	Corners c = getCorners(toBuild, buildSpot);

	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BUILDABLE)
				{
					cover_map[x][y] = TEMPBLOCKED;
				}
			}
		}
	}
}

void CoverMap::clear(Corners c)
{
	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == BLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
			}
		}
	}
}

void CoverMap::clearTemp(UnitType toBuild, TilePosition buildSpot)
{
	if (buildSpot.x() == -1)
	{
		return;
	}

	Corners c = getCorners(toBuild, buildSpot);

	for (int x = c.x1; x <= c.x2; x++)
	{
		for (int y = c.y1; y <= c.y2; y++)
		{
			if (x >= 0 && x < w && y >= 0 && y < h)
			{
				if (cover_map[x][y] == TEMPBLOCKED)
				{
					cover_map[x][y] = BUILDABLE;
				}
			}
		}
	}
}

Corners CoverMap::getCorners(Unit* unit)
{
	return getCorners(unit->getType(), unit->getTilePosition());
}

Corners CoverMap::getCorners(UnitType type, TilePosition center)
{
	int x1 = center.x();
	int y1 = center.y();
	int x2 = x1 + type.tileWidth() - 1;
	int y2 = y1 + type.tileHeight() - 1;

	int margin = 1;
	if (type.canProduce())
	{
		margin = 1;
	}

	if (BaseAgent::isOfType(type, UnitTypes::Terran_Supply_Depot))
	{
		margin = 0;
	}
	if (BaseAgent::isOfType(type, UnitTypes::Protoss_Pylon))
	{
		margin = 0;
	}

	x1 -= margin;
	x2 += margin;
	y1 -= margin;
	y2 += margin;

	//Special case: Terran Addon buildings
	//Add 2 extra spaces to the right to make space for the addons.
	if (BaseAgent::isOfType(type, UnitTypes::Terran_Factory) || BaseAgent::isOfType(type, UnitTypes::Terran_Starport) || BaseAgent::isOfType(type, UnitTypes::Terran_Command_Center) || BaseAgent::isOfType(type, UnitTypes::Terran_Science_Facility))
	{
		x2 += 2;
	}

	Corners c;
	c.x1 = x1;
	c.y1 = y1;
	c.x2 = x2;
	c.y2 = y2;

	return c;
}

TilePosition CoverMap::findRefineryBuildSpot(UnitType toBuild, TilePosition start)
{
	TilePosition buildSpot = findClosestGasWithoutRefinery(toBuild, start);
	if (buildSpot.x() >= 0)
	{
		BaseAgent* base = AgentManager::Instance().getClosestBase(buildSpot);
		if (base == NULL)
		{
			Broodwar->printf("No base found");
			return TilePosition(-1,-1);
		}
		else
		{
			double dist = buildSpot.getDistance(base->getUnit()->getTilePosition());
			if (dist >= 13) 
			{
				Broodwar->printf("Base too far away %d", (int)dist);
				return TilePosition(-1,-1);
			}
		}

	}
	return buildSpot;
}

TilePosition CoverMap::findClosestGasWithoutRefinery(UnitType toBuild, TilePosition start)
{
	TilePosition bestSpot = TilePosition(-1,-1);
	double bestDist = -1;
	TilePosition home = Broodwar->self()->getStartLocation();
	Unit* worker = findWorker();

	for(int i = 0 ; i < w ; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (cover_map[i][j] == GAS)
			{
				TilePosition cPos = TilePosition(i,j);

				bool ok = true;
				vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					Unit* unit = agents.at(i)->getUnit();
					if (unit->getType().isRefinery())
					{
						double dist = unit->getTilePosition().getDistance(cPos);
						if (dist <= 2)
						{
							ok = false;
						}
					}
				}
				if (ok)
				{
					if (ExplorationManager::canReach(home, cPos))
					{
						BaseAgent* agent = AgentManager::Instance().getClosestBase(cPos);
						double dist = agent->getUnit()->getTilePosition().getDistance(cPos);
						if (bestDist == -1 || dist < bestDist)
						{
							bestDist = dist;
							bestSpot = cPos;
						}
					}
				}
			}
		}
	}

	return bestSpot;
}

TilePosition CoverMap::searchRefinerySpot()
{
	for(int i = 0 ; i < w ; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (cover_map[i][j] == GAS)
			{
				TilePosition cPos = TilePosition(i,j);

				bool found = false;
				vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
				for (int i = 0; i < (int)agents.size(); i++)
				{
					if (agents.at(i)->getUnitType().isRefinery())
				{
						double dist = agents.at(i)->getUnit()->getTilePosition().getDistance(cPos);
						TilePosition uPos = agents.at(i)->getUnit()->getTilePosition();
						if (dist <= 2)
				{
							found = true;
							break;
						}
					}
				}

				if (!found)
				{
					BaseAgent* agent = AgentManager::Instance().getClosestBase(cPos);
					if (agent != NULL)
					{
						TilePosition bPos = agent->getUnit()->getTilePosition();
						double dist = bPos.getDistance(cPos);

						if (dist < 15)
						{
							if (ExplorationManager::canReach(bPos, cPos))
							{
								return cPos;
							}			
						}
					}
				}
			}
		}
	}

	return TilePosition(-1, -1);
}

TilePosition CoverMap::findExpansionSite()
{
	/**
		Find the expansion site that is:
		1. Not occupied by a known enemy
		2. Not occupied by an expansion of our own
		3. Is closest to any of our own bases
	*/

	TilePosition result(-1, -1);

	const std::set<BWTA::BaseLocation*>& baseLocations = BWTA::getBaseLocations();
	std::set<BWTA::BaseLocation*>::const_iterator it;

	UnitType baseType = Broodwar->self()->getRace().getCenter();

	double bestWeight = 1000000.0;
	for (it = baseLocations.begin(); it != baseLocations.end(); it++)
	{
		TilePosition expansionPosition = (*it)->getTilePosition();

		// Check whether there are enemy structures near
		if (ExplorationManager::Instance().spottedBuildingsWithinRange(expansionPosition, 20) > 0)
			continue;

		// Check whether we can build there
		TilePosition expansionBuildSpot = findClosestBuildSpot(baseType, expansionPosition);
		if (expansionBuildSpot.x() < 0)
			continue;
		
		// Check whether we already have an expansion at the site (TODO: Find a better way of detecting this)
		BaseAgent* closestExpansion = AgentManager::Instance().getClosestBase(expansionBuildSpot);
		TilePosition closestExpansionLocation(-1, -1);
		if (closestExpansion != NULL)
		{
			closestExpansionLocation = closestExpansion->getUnit()->getTilePosition();
			if (expansionBuildSpot.getDistance(closestExpansionLocation) < 10)
				continue;
		}
		
		/** Location is valid, weight it according to:
			1. How close is it to a mineral field
			2. How close is it to one of our existing bases
		*/

		Unit* closestMineral = findClosestMineral(expansionBuildSpot);
		if (closestMineral == NULL)
			continue;
		
		double distanceToMineral = mapData.getDistance(expansionBuildSpot, closestMineral->getTilePosition());
		double distanceToExistingExpansion = 0;
		if (closestExpansionLocation.x() >= 0)
			distanceToExistingExpansion = mapData.getDistance(expansionBuildSpot, closestExpansionLocation);

		// Normalize the inputs
		double maxDistance = Broodwar->mapWidth() * Broodwar->mapWidth() + Broodwar->mapHeight() * Broodwar->mapHeight();

		// Smallest weight is best weight
		double weight = (1.0 * distanceToMineral / maxDistance + 
						 1.5 * distanceToExistingExpansion / maxDistance);
		if (weight < bestWeight)
		{
			bestWeight = weight;
			result = expansionBuildSpot;
		}
	}

	return result;
}

Unit* CoverMap::findBestMiningField(TilePosition workerPos)
{
	/**
		List all mineral fields and weight them according to:
		1. How close they are to an existing base (prioritize mineral fields close to a base)
		2. How many workers there are at the base already (prioritize working at bases with few workers)
		3. How close they are to the workerPos (prioritize moving a location close to us)
	*/
	Unit* mineral = NULL;
	std::set<BWAPI::Unit*> mineralFields = Broodwar->getMinerals();
	std::set<BWAPI::Unit*>::iterator it;

	double bestWeight = 1000000.0;
	for (it = mineralFields.begin(); it != mineralFields.end(); it++)
	{
		TilePosition mineralFieldPos = (*it)->getTilePosition();

		BaseAgent* nearestBase = AgentManager::Instance().getClosestBase(mineralFieldPos);

		double distanceToField = mapData.getDistance(workerPos, mineralFieldPos);
		double distanceToBase = mapData.getDistance(workerPos, nearestBase->getUnit()->getTilePosition());
		//double workerCountApproximation = (*it)->getUnitsInRadius(10).size();

		// Normalize the inputs
		double maxDistance = Broodwar->mapWidth() * Broodwar->mapWidth() + Broodwar->mapHeight() * Broodwar->mapHeight();
		//int maxWorkers = AgentManager::Instance().getNoWorkers();

		// Lower is better
		double weight = (1.0 * distanceToField / maxDistance + 
						 10.0 * distanceToBase / maxDistance); //+ 
						 //2.0 * workerCountApproximation / maxWorkers);
		if (weight < bestWeight)
		{
			bestWeight = weight;
			mineral = (*it);
		}
	}

	return mineral;
}

Unit* CoverMap::findClosestMineral(const TilePosition& position)
{
	Unit* mineral = NULL;
	double bestDistance = 1000000.0;
	for (std::set<BWAPI::Unit*>::iterator m = Broodwar->getMinerals().begin(); m != Broodwar->getMinerals().end(); m++)
	{
		//double distance = (*m)->getTilePosition().getDistance(position);
		double distance = mapData.getDistance((*m)->getTilePosition(), position);
		if (distance < bestDistance)
		{
			bestDistance = distance;
			mineral = (*m);
		}
	}

	return mineral;
}

bool CoverMap::suitableForDetector(TilePosition pos)
{
	vector<BaseAgent*> agents = AgentManager::Instance().getAgents();
	for (int i = 0; i < (int)agents.size(); i++)
	{
		BaseAgent* agent = agents.at(i);
		UnitType type = agent->getUnitType();
		if (agent->isAlive() && type.isDetector() && type.isBuilding())
		{
			double range = type.sightRange() * 1.6;
			double dist = agent->getUnit()->getPosition().getDistance(Position(pos));
			if (dist <= range)
			{
				return false;
			}
		}
	}
	return true;
}

void CoverMap::debug()
{
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (cover_map[x][y] == TEMPBLOCKED)
			{
				Broodwar->drawBox(CoordinateType::Map,x*32,y*32,x*32+31,y*32+31,Colors::Green,false);
			}
			if (cover_map[x][y] == BLOCKED)
			{
				Broodwar->drawBox(CoordinateType::Map,x*32,y*32,x*32+31,y*32+31,Colors::Red,false);
			}
		}
	}
}
