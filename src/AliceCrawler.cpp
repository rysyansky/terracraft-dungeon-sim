#include "AliceCrawler.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>

void AliceCrawler::Initialize(int start_room, int initial_food) {
    this->initial_food = initial_food;
    this->exploring = true;
    this->collected_in_room.clear();
    this->action_queue.clear();
}

void AliceCrawler::Reset() {
    action_queue.clear();
    exploring = true;
    collected_in_room.clear();
}

BotAction AliceCrawler::DecideNextStep(const BotState& state) {
    bool was_exploring = exploring;
    if (action_queue.empty()) {
        if (exploring) FillExplorationActions(state);
        else FillReturnActions(state);
    }

    if (!action_queue.empty()) {
        BotAction act = action_queue.front();
        action_queue.pop_front();
        return act;
    }

    if (was_exploring) {
        exploring = false;
        return BotAction{BotAction::SWITCH, -1};
    }
    return BotAction{BotAction::FINISH, -1};
}

void AliceCrawler::FillExplorationActions(const BotState& state) {
    int cur = state.current_room;

    if (collected_in_room.find(cur) == collected_in_room.end()) {
        Resource best = GetHighestValueResource(cur, state);
        if (state.known_room_resources.at(cur).at(best) > 0) {
            action_queue.push_back(BotAction{BotAction::COLLECT, cur, best});
            collected_in_room.insert(cur);
            return;
        }
        collected_in_room.insert(cur);
    }

    if (initial_food - state.remaining_food >= initial_food / 2) {
        exploring = false;
        action_queue.clear();
        return;
    }

    int next = -1;
    std::vector<int> adj = state.GetNeighbors(cur);
    std::sort(adj.begin(), adj.end());

    for (int n : adj) {
        if (state.visited_rooms.find(n) == state.visited_rooms.end()) {
            next = n;
            break;
        }
    }

    if (next == -1) next = FindNearestUnvisited(state);

    if (next != -1) {
        std::vector<int> path = BuildShortestPath(state, next, true);
        for (int r : path) action_queue.push_back(BotAction{BotAction::MOVE, r});
    }
}

void AliceCrawler::FillReturnActions(const BotState& state) {
    std::vector<int> path = BuildShortestPath(state, 0, false);
    if (path.empty()) {
        action_queue.push_back(BotAction{BotAction::FINISH, -1});
        return;
    }

    if (!path.empty() && path.back() == 0) {
        path.pop_back();
    }

    if (path.empty()) {
        action_queue.push_back(BotAction{BotAction::FINISH, -1});
        return;
    }

    int excess = state.remaining_food - static_cast<int>(path.size()) - 1;
    PlanCollectionsOnPath(state, path, excess);
}

Resource AliceCrawler::GetHighestValueResource(int room_id, const BotState& state) const {
    Resource best = Resource::Iron;
    int max_val = -1;
    auto& res_map = state.known_room_resources.at(room_id);
    
    for (const auto& [res, count] : res_map) {
        if (count == 0) continue;
        int base = Dungeon().costs.at(res);
        int val = base + (res == state.target_resource ? base : 0);
        if (val > max_val) {
            max_val = val;
            best = res;
        }
    }
    return best;
}

int AliceCrawler::FindNearestUnvisited(const BotState& state) const {
    std::queue<int> q;
    std::unordered_set<int> seen;
    q.push(state.current_room);
    seen.insert(state.current_room);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        std::vector<int> adj = state.GetNeighbors(u);
        std::sort(adj.begin(), adj.end());
        for (int v : adj) {
            if (state.visited_rooms.find(v) == state.visited_rooms.end()) return v;
            if (seen.find(v) == seen.end()) {
                seen.insert(v);
                q.push(v);
            }
        }
    }
    return -1;
}

std::vector<int> AliceCrawler::BuildShortestPath(const BotState& state, int target, bool allow_unvisited) const {
    std::queue<int> q;
    std::unordered_map<int, int> parent;
    std::unordered_set<int> seen;
    q.push(state.current_room);
    seen.insert(state.current_room);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == target) break;

        std::vector<int> adj = state.GetNeighbors(u);
        std::sort(adj.begin(), adj.end());

        for (int v : adj) {
            bool ok = state.visited_rooms.count(v) || (allow_unvisited && v == target);
            if (ok && seen.find(v) == seen.end()) {
                seen.insert(v);
                parent[v] = u;
                q.push(v);
            }
        }
    }

    if (seen.find(target) == seen.end()) return {};
    std::vector<int> path;
    for (int c = target; c != state.current_room; c = parent.at(c)) path.push_back(c);
    std::reverse(path.begin(), path.end());
    return path;
}

void AliceCrawler::PlanCollectionsOnPath(const BotState& state, std::vector<int>& path, int& excess) {
    for (size_t i = 0; i < path.size(); ++i) {
        int room = path[i];
        action_queue.push_back(BotAction{BotAction::MOVE, room});

        auto& res_map = state.known_room_resources.at(room);
        std::vector<Resource> sorted = {Resource::Gems, Resource::Gold, Resource::Iron, Resource::Exp};
        auto target = state.target_resource;
        std::sort(sorted.begin(), sorted.end(), [&](Resource a, Resource b) {
            int va = Dungeon().costs.at(a) + (a == target ? Dungeon().costs.at(a) : 0);
            int vb = Dungeon().costs.at(b) + (b == target ? Dungeon().costs.at(b) : 0);
            return va > vb;
        });

        int collected_cnt = collected_in_room.count(room) ? collected_in_room.count(room) : 0;
        for (Resource r : sorted) {
            if (res_map.at(r) <= 0) continue;
            int cost = (collected_cnt == 0) ? 0 : 1;
            if (excess >= cost) {
                action_queue.push_back(BotAction{BotAction::COLLECT, -1, r});
                excess -= cost;
                collected_cnt++;                
            } else break;
        }
    }
}