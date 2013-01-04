#include <Commander\Tactic.hpp>
#include <Commander\SquadFileReader.h>
#include <Utils\FileReaderUtils.h>
#include <Managers\ExplorationManager.h>
#include <Managers\BuildPlanner.h>
#include <Commander\MySpecialSquad.h>
#include <Commander\ExplorationSquad.h>
#include <fstream>
#include <sstream>

SquadFileReader::SquadFileReader()
{
	id = 1;
}

vector<Squad*> SquadFileReader::readSquadList()
{
	string filename = getFilename("squads");
	
	//Read buildorder file
	ifstream inFile;

	stringstream ss;
	ss << getScriptPath();
	ss << "squads\\";
	ss << filename;
	string filePath = ss.str();

	inFile.open(filePath.c_str());

	if (!inFile)
	{
		Broodwar->printf("Unable to open file %s", filePath.c_str());
	}
	else
	{
		string line;
		char buffer[256];

		while (!inFile.eof())
		{
			inFile.getline(buffer, 100);
			if (buffer[0] != ';')
			{
				stringstream ss;
				ss << buffer;
				line = ss.str();
				
				Tokens token = split(line, "=");
				
				if (token.key == "Type")
				{
					type = token.value;
				}
				if (token.key == "Name")
				{
					name = token.value;
				}
				if (token.key == "MorphsTo")
				{
					morphsTo = getUnitType(token.value);
					//Broodwar->printf("FRU %s", morphsTo.getName().c_str());
				}
				if (token.key == "Priority")
				{
					priority = toInt(token.value);
					activePriority = priority;
				}
				if (token.key == "ActivePriority")
				{
					activePriority = toInt(token.value);
				}
				if (token.key == "OffenseType")
				{
					offType = token.value;
				}
				if (line == "<setup>")
				{
					createSquad();
				}
				if (token.key == "Unit")
				{
					addUnit(token.value);
				}
			}
		}
		inFile.close();
	}

	Broodwar->printf("Squad file %s loaded. %d squads added.", filePath.c_str(), (int)squads.size());
	
	return squads;
}

void SquadFileReader::createSquad()
{
	bool required = false;
	if (offType == "Required")
	{
		required = true;
	}

	Tactic* defaultTactic = Tactic::createTacticFromName(type);
	cSquad = new Squad(id, defaultTactic, name, priority, activePriority, required);

	squads.push_back(cSquad);
	id++;
}

void SquadFileReader::addUnit(string line)
{
	Tokens token = split(line, ":");

	//Replace all _ with whitespaces, or they wont match
	replace(token.key);
	
	UnitType type = getUnitType(token.key);
	if (type.getID() != UnitTypes::Unknown.getID())
	{
		int no = toInt(token.value);
		cSquad->getSetup().addRequirement(type, toInt(token.value));
		return;
	}

	//No UnitType match found
	Broodwar->printf("Error: No matching UnitType found for %s", token.key.c_str());
}
