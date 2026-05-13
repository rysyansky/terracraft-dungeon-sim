#pragma once

#include "Dungeon.hpp"
#include "FogOfWar.hpp"
#include "IBotStrategy.hpp"
#include <map>
#include <set>
#include <vector>

class GameSimulator{
public:
    GameSimulator(const Dungeon &dungeon, IBotStrategy &bot_strategy);
    void Run();
    std::vector<std::string> GetLog() const { return log; }
private:
    IBotStrategy &bot_strategy;
    FogOfWar fow;
    std::vector<std::string> log;
    void WriteLog(const BotState &state, const BotAction &action, 
        const std::map<int, std::set<Resource>>& collected_tracker);
    Resource FindMostValuableResource(std::map<Resource, int> resources);
    int CalculateFinalValue(std::map<Resource, int> resources) const;
};