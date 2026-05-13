#pragma once
#include <map>
#include <string>
#include <vector>

enum Resource : int {
    Iron = 0,
    Gold = 1,
    Gems = 2,
    Exp = 3
};

struct Dungeon {
    int room_count = 0;
    int food = 0;
    Resource m_double_cost = Resource::Iron;
    std::vector<std::string> lines;
    std::vector<std::map<Resource, int>> resources_in_room;
    std::vector<std::vector<int>> room_graph;
    std::map<Resource, int> costs = {
        {Resource::Iron, 7},
        {Resource::Gold, 11},
        {Resource::Gems, 23},
        {Resource::Exp, 1}
    };
    explicit Dungeon(int total_rooms) 
        : room_count(total_rooms), 
          resources_in_room(total_rooms), 
          room_graph(total_rooms) {}
    Dungeon() = default;
};