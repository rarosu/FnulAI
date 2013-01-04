#include <UnitAgents\TransportAgent.h>
#include <PotentialFields\PFManager.h>
#include <Managers\AgentManager.h>
#include <Commander\Commander.h>
#include <Commander\Squad.h>

TransportAgent::TransportAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	maxLoad = type.spaceProvided();
	currentLoad = 0;
	unitID = unit->getID();
	agentType = "TransportAgent";
	
	goal = TilePosition(-1, -1);
}

int TransportAgent::getCurrentLoad()
{
	Squad* sq = Commander::Instance().getSquad(squadID);
	if (sq != NULL)
	{
		int load = 0;
		vector<BaseAgent*> agents = sq->getUnits().getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* a = agents.at(i);
			if (a->isAlive())
			{
				if (a->getUnit()->isLoaded())
				{
					if (a->getUnit()->getTransport()->getID() == unit->getID())
					{
						load += a->getUnitType().spaceRequired();
					}
				}
			}
		}
		currentLoad = load;
	}

	return currentLoad;
}

bool TransportAgent::isValidLoadUnit(BaseAgent* a)
{
	if (a->getUnitType().isFlyer()) return false;
	if (a->getUnit()->isLoaded()) return false;
	if (a->getUnit()->isBeingConstructed()) return false;
	if (a->getUnitID() == unit->getID()) return false;
	return true;
}

BaseAgent* TransportAgent::findUnitToLoad(int spaceLimit)
{
	BaseAgent* agent = NULL;
	double bestDist = 100000;

	Squad* sq = Commander::Instance().getSquad(squadID);
	if (sq != NULL)
	{
		vector<BaseAgent*> agents = sq->getUnits().getAgents();
		for (int i = 0; i < (int)agents.size(); i++)
		{
			BaseAgent* a = agents.at(i);
			if (isValidLoadUnit(a))
			{
				double cDist = unit->getPosition().getDistance(a->getUnit()->getPosition());
				if (cDist < bestDist)
				{
					bestDist = cDist;
					agent = a;
				}
			}
		}
	}

	return agent;
}

void TransportAgent::computeActions()
{
	if (unit->isBeingConstructed()) return;

	int currentLoad = getCurrentLoad();
	int eCnt = enemyUnitsWithinRange(unit->getType().sightRange());

	if (eCnt == 0)
	{
		if (currentLoad < maxLoad)
		{
			BaseAgent* toLoad = findUnitToLoad(maxLoad - currentLoad);
			if (toLoad != NULL)
			{
				unit->load(toLoad->getUnit());
				return;
			}
		}
	}
	else
	{
		if (currentLoad > 0)
		{
			TilePosition t = unit->getTilePosition();
			unit->unloadAll();
			return;
		}
	}

	bool defensive = true;
	PFManager::Instance().computeAttackingUnitActions(this, goal, defensive);
}
