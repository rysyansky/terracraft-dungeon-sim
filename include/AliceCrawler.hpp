#pragma once

#include "IBotStrategy.hpp"
#include "Dungeon.hpp"

#include <vector>
#include <deque>
#include <unordered_set>

class AliceCrawler : public IBotStrategy {
public:
    void Initialize(int start_room, int initial_food) override;
    BotAction DecideNextStep(const BotState& state) override;
    void Reset() override;

private:
    std::deque<BotAction> action_queue;
    std::unordered_set<int> collected_in_room;
    bool exploring = true;
    int initial_food = 0;

    void FillExplorationActions(const BotState& state);
    void FillReturnActions(const BotState& state);
    Resource GetHighestValueResource(int room_id, const BotState& state) const;
    int FindNearestUnvisited(const BotState& state) const;
    std::vector<int> BuildShortestPath(const BotState& state, int target, bool allow_unvisited) const;
    void PlanCollectionsOnPath(const BotState& state, std::vector<int>& path, int& excess_food);
};