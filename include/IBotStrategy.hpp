#pragma once

#include "Dungeon.hpp"
#include <unordered_set>
#include <vector>

struct BotState{
    int current_room = 0;
    int remaining_food = 0;
    Resource target_resource = Resource::Iron;
    bool exploring = true;
    std::map<Resource, int> collected_resources = {};
    std::map<int, std::map<Resource, int>> known_room_resources;
    std::unordered_set<int> visited_rooms = {};
    std::vector<std::vector<int>> known_adj = {};

    std::vector<int> GetNeighbors(int room_id) const {
        if (room_id < 0 || room_id >= static_cast<int>(known_adj.size())) {
            return {};
        }
        return known_adj[room_id];
    }
};

struct BotAction {
    enum Type { START, MOVE, COLLECT, FINISH, SWITCH, DEATH } type;
    int target_room = -1;
    Resource target = Resource::Iron;
};

struct IBotStrategy {
    virtual ~IBotStrategy() = default;
    
    virtual void Initialize(int start_room, int initial_food) = 0;
    
    virtual BotAction DecideNextStep(const BotState& state) = 0;
    
    virtual void Reset() = 0;
};