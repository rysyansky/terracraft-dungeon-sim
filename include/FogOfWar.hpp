#pragma once

#include "Dungeon.hpp"
#include <unordered_set>
#include <vector>

enum RoomVisibility {
    UNKNOWN,
    VISIBLE,
    EXPLORED
};

struct RoomInfo {
    RoomVisibility visibility = RoomVisibility::UNKNOWN;
    std::map<Resource, int> resources;
    std::vector<int> neighbors;
};

class FogOfWar {
public:
    FogOfWar(const Dungeon &dungeon);
    ~FogOfWar();

    void VisitRoom(int room_id);
    RoomInfo GetRoomInfo(int room_id) const;
    int GetRoomCount() const;
    void PutResourcesInRoom(
        int iron,
        int gold,
        int gems,
        int exp,
        int room_id
    );
    std::vector<int> GetNearbyRooms(int room_id) const;
    std::map<Resource, int> GetResourcesInRoom(int room_id) const;
    int GetFood() const;
    Resource GetDoubleCost() const;
    int GetResourceCost(Resource res) const;
    std::vector<std::vector<int>> GetKnownAdj() const;
    std::unordered_set<int> GetVisitedRooms() const;
    std::map<int, std::map<Resource, int>> GetKnownRoomResources() const;
private:
    const Dungeon &dungeon;
    std::map<int, RoomInfo> room_info;
};