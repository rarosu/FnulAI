#include <Utils\Pathfinder.h>
#include <Managers\ExplorationManager.h>
#include <Utils\Profiler.h>
#include <fstream>
#include <sstream>


Pathfinder::Pathfinder()
{
	running = true;
	lock = false;
	end = false;
	CreateThread();
}

Pathfinder::~Pathfinder()
{
	running = false;
	while (!end)
	{
		Sleep(1);
	}

	for (int i = 0; i < (int)pathObj.size(); i++)
	{	
		delete pathObj.at(i);
	}
}


PathObj* Pathfinder::getPathObj(TilePosition start, TilePosition end)
{
	for (int i = 0; i < (int)pathObj.size(); i++) {
		if (pathObj.at(i)->matches(start, end))
		{
			return pathObj.at(i);
		}
	}
	return NULL;
}

int Pathfinder::getDistance(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		if (obj->isFinished())
		{
			return obj->getPath().size();
		}
	}
	return 0;
}

void Pathfinder::requestPath(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj == NULL) 
	{
		obj = new PathObj(start, end);
		pathObj.push_back(obj);
	}

	if ((int)pathObj.size() > 20)
	{
		lock = true;

		while (pathObj.size() > 20)
		{
			pathObj.erase(pathObj.begin());
		}

		lock = false;
	}
}

bool Pathfinder::isReady(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		return obj->isFinished();
	}
	return false;
}

vector<TilePosition> Pathfinder::getPath(TilePosition start, TilePosition end)
{
	PathObj* obj = getPathObj(start, end);
	if (obj != NULL)
	{
		if (obj->isFinished())
		{
			return obj->getPath();
		}
	}
	return vector<TilePosition>();
}

void Pathfinder::stop()
{
	running = false;
}

bool Pathfinder::isRunning()
{
	if (!Broodwar->isInGame()) running = false;
	return running;
}

unsigned long Pathfinder::Process (void* parameter)
{
	while (running)
	{
		for (int i = 0; i < (int)pathObj.size(); i++)
		{
			if (!isRunning()) break;
			while (lock);
			if (!isRunning()) break;
			if (!pathObj.at(i)->isFinished())
			{
				if (!isRunning()) break;
				pathObj.at(i)->calculatePath();
			}
		}
		Sleep(5);
	}

	end = true;

	return 0;
}

