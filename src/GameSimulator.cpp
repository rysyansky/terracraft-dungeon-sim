#include "GameSimulator.hpp"
#include "Dungeon.hpp"
#include "IBotStrategy.hpp"
#include <stdexcept>
GameSimulator::GameSimulator(const Dungeon &dungeon, IBotStrategy &bot_strategy)
    : bot_strategy(bot_strategy), fow(dungeon) {}

void GameSimulator::Run() {
    BotState state;
    state.current_room = 0;
    state.remaining_food = fow.GetFood();
    state.exploring = true;
    state.target_resource = fow.GetDoubleCost();
    state.known_adj = fow.GetKnownAdj();
    state.visited_rooms.insert(0);
    state.collected_resources = {
        {Resource::Iron, 0}, {Resource::Gold, 0},
        {Resource::Gems, 0}, {Resource::Exp, 0}
    };

    state.known_room_resources = fow.GetKnownRoomResources();
    bot_strategy.Initialize(0, state.remaining_food);

    std::unordered_set<int> first_collect_done;
    std::map<int, std::set<Resource>> collected_resources_tracker;

    BotAction action = bot_strategy.DecideNextStep(state);

    while (action.type != BotAction::FINISH) {
        if (state.remaining_food <= 0 && state.current_room != 0) {
            action.type = BotAction::DEATH;
            break;
        }

        switch (action.type) {
            case BotAction::MOVE: {
                if (action.target_room < 0 || action.target_room >= fow.GetRoomCount())
                    throw std::runtime_error("Invalid target room");

                state.remaining_food--;
                state.current_room = action.target_room;
                fow.VisitRoom(action.target_room);
                state.known_adj = fow.GetKnownAdj();
                state.visited_rooms = fow.GetVisitedRooms();
                state.visited_rooms.insert(state.current_room);
                state.known_room_resources = fow.GetKnownRoomResources();

                WriteLog(state, action, collected_resources_tracker);
                break;
            }

            case BotAction::COLLECT: {
                Resource res = static_cast<Resource>(action.target);
                auto room_res = fow.GetResourcesInRoom(state.current_room);
                int amount = room_res.at(res);
                if (amount <= 0) break;

                state.collected_resources[res] += amount;

                bool is_free = first_collect_done.find(state.current_room) == first_collect_done.end();
                if (!is_free) {
                    state.remaining_food--;
                }
                first_collect_done.insert(state.current_room);
                collected_resources_tracker[state.current_room].insert(res);

                fow.PutResourcesInRoom(
                    (res == Resource::Iron ? 0 : room_res.at(Resource::Iron)),
                    (res == Resource::Gold ? 0 : room_res.at(Resource::Gold)),
                    (res == Resource::Gems ? 0 : room_res.at(Resource::Gems)),
                    (res == Resource::Exp  ? 0 : room_res.at(Resource::Exp)),
                    state.current_room
                );

                WriteLog(state, action, collected_resources_tracker);
                break;
            }

            case BotAction::SWITCH: {
                state.exploring = false;
                break;
            }

            default:
                throw std::runtime_error("Unknown action type");
        }

        if (state.remaining_food <= 0 && state.current_room != 0) {
            action.type = BotAction::DEATH;
            break;
        }

        action = bot_strategy.DecideNextStep(state);
    }

    if (action.type == BotAction::FINISH) {
        WriteLog(state, BotAction{BotAction::FINISH, 0}, collected_resources_tracker);
    } else {
        log.push_back("death");
    }
}

void GameSimulator::WriteLog(const BotState &state, const BotAction &action, 
                             const std::map<int, std::set<Resource>>& collected_tracker) {
    if (action.type == BotAction::MOVE) {
        log.push_back("go " + std::to_string(action.target_room));
    } else if (action.type == BotAction::COLLECT) {
        static const std::map<int, std::string> names = {
            {0, "iron"}, {1, "gold"}, {2, "gems"}, {3, "exp"}
        };
        log.push_back("collect " + names.at(action.target));
    } else if (action.type == BotAction::FINISH) {
        log.push_back("go 0");
        log.push_back("result " + 
                      std::to_string(state.collected_resources.at(Resource::Iron)) + " " +
                      std::to_string(state.collected_resources.at(Resource::Gold)) + " " +
                      std::to_string(state.collected_resources.at(Resource::Gems)) + " " +
                      std::to_string(state.collected_resources.at(Resource::Exp)) + " " + 
                      std::to_string(CalculateFinalValue(state.collected_resources)));
        return;
    }

    std::string line = "state " + std::to_string(state.current_room);
    for (int r = 0; r < 4; ++r) {
        Resource res = static_cast<Resource>(r);
        bool is_collected = collected_tracker.count(state.current_room) && 
                            collected_tracker.at(state.current_room).count(res);
        
        // Формат: пробел + значение или "_"
        line += is_collected ? " _" : " " + std::to_string(fow.GetResourcesInRoom(state.current_room).at(res));
    }
    log.push_back(line);
}

Resource GameSimulator::FindMostValuableResource(std::map<Resource, int> resources) {
    int max_value = -1;
    Resource mvr = Resource::Iron;
    for(const auto& [res, amount] : resources) {
        int value = fow.GetResourceCost(res) * amount;
        if(value > max_value) {
            max_value = value;
            mvr = res;
        }
    }
    return mvr;
}

int GameSimulator::CalculateFinalValue(std::map<Resource, int> resources) const {
    int cost = 0;
    for(const auto& [res, amount] : resources) {
        cost += fow.GetResourceCost(res) * amount;
    }
    return cost;
}