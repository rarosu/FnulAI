#ifndef TACTIC_HPP
#define TACTIC_HPP

#include <string>

class Tactic
{
public:
	virtual void computeActions() = 0;

	static Tactic* createTacticFromName(const std::string& name);

	static const std::string OFFENSIVE;
	static const std::string DEFENSIVE;
	static const std::string EXPLORATION;
	static const std::string BUNKER;
};

#endif