#include <Commander\Tactic.hpp>

const std::string Tactic::OFFENSIVE("Offensive");
const std::string Tactic::DEFENSIVE("Defensive");
const std::string Tactic::EXPLORATION("Exploration");
const std::string Tactic::BUNKER("Bunker");

Tactic* Tactic::createTacticFromName(const std::string& name)
{
	return NULL;
}