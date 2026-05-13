#include "FogOfWar.hpp"
#include <stdexcept>

FogOfWar::FogOfWar(const Dungeon &dungeon) : dungeon(dungeon) {
    for(int i = 0; i < this->dungeon.room_count; ++i) {
        room_info[i] = RoomInfo();
    }
    VisitRoom(0);
}

FogOfWar::~FogOfWar() {}

void FogOfWar::VisitRoom(int room_id) {
    if (room_info.count(room_id) == 0) throw std::runtime_error("Invalid room ID");

    RoomInfo &info = room_info.at(room_id);
    if(info.visibility != RoomVisibility::EXPLORED) {
        info.visibility = RoomVisibility::EXPLORED;
        info.resources = dungeon.resources_in_room[room_id];
        info.neighbors = dungeon.room_graph[room_id];
    }

    for (int adj : info.neighbors) {
        auto adj_it = room_info.find(adj);
        if (adj_it != room_info.end() && adj_it->second.visibility == RoomVisibility::UNKNOWN) {
            adj_it->second.visibility = RoomVisibility::VISIBLE;
        }
    }
}

RoomInfo FogOfWar::GetRoomInfo(int room_id) const {
    if (room_info.count(room_id) == 0) throw std::runtime_error("Invalid room ID");
    return room_info.at(room_id);
}

int FogOfWar::GetRoomCount() const {
    return dungeon.room_count;
}

void FogOfWar::PutResourcesInRoom(
    int iron,
    int gold,
    int gems,
    int exp,
    int room_id
) {
    if (room_info.count(room_id) == 0) throw std::runtime_error("Invalid room ID");
    room_info[room_id].resources[Resource::Iron] = iron;
    room_info[room_id].resources[Resource::Gold] = gold;
    room_info[room_id].resources[Resource::Gems] = gems;
    room_info[room_id].resources[Resource::Exp] = exp;
}
std::vector<int> FogOfWar::GetNearbyRooms(int room_id) const {
    if (room_info.count(room_id) == 0) throw std::runtime_error("Invalid room ID");
    return room_info.at(room_id).neighbors;
}
std::map<Resource, int> FogOfWar::GetResourcesInRoom(int room_id) const {
    if (room_info.count(room_id) == 0) throw std::runtime_error("Invalid room ID");
    return room_info.at(room_id).resources;
}
int FogOfWar::GetFood() const {
    return dungeon.food;
}
Resource FogOfWar::GetDoubleCost() const {
    return dungeon.m_double_cost;
}
int FogOfWar::GetResourceCost(Resource res) const {
    if (dungeon.costs.count(res) == 0) throw std::runtime_error("Invalid resource");
    return res == dungeon.m_double_cost ? 2 * dungeon.costs.at(res) : dungeon.costs.at(res);
}

std::vector<std::vector<int>> FogOfWar::GetKnownAdj() const {
    std::vector<std::vector<int>> known_adj(dungeon.room_count);
    for (const auto& [room_id, info] : room_info) {
        if (info.visibility != RoomVisibility::UNKNOWN) {
            known_adj[room_id] = info.neighbors;
        }
    }
    return known_adj;
}

std::unordered_set<int> FogOfWar::GetVisitedRooms() const {
    std::unordered_set<int> visited;
    for (const auto& [room_id, info] : room_info) {
        if (info.visibility == RoomVisibility::EXPLORED) {
            visited.insert(room_id);
        }
    }
    return visited;
}

std::map<int, std::map<Resource, int>> FogOfWar::GetKnownRoomResources() const {
    std::map<int, std::map<Resource, int>> known_resources;
    for (const auto& [room_id, info] : room_info) {
        if (info.visibility != RoomVisibility::UNKNOWN) {
            known_resources[room_id] = info.resources;
        }
    }
    return known_resources;
}