#include "Reader.hpp"
#include "Dungeon.hpp"
#include <algorithm>
#include <sstream>
#include <string>

Reader::Reader(const std::string& filename) : in(filename) {
    if(in.is_open() == false) {
        throw std::runtime_error("File " + filename + " did not open!\n");
    }
}
Reader::~Reader() {
    in.close();
}

Dungeon Reader::GetDungeon() {
    const auto check_is_digit = [](const std::string& s) {
        return std::all_of(s.begin(), s.end(), [](char c){ return std::isdigit(c); });
    };
    std::string line;
    std::getline(in, line);
    
    if(check_is_digit(line) == false) {
        throw std::runtime_error(line);
    }
    int room_count = std::stoi(line);
    if(Validate(1, 255, room_count) == false) {
        throw std::runtime_error(line);
    }

    dungeon = Dungeon(room_count + 1);

    for(int i = 0; i < room_count + 1; i++) {
        std::getline(in, line);
        std::stringstream reader_ss(line);
        std::string cur_room, raw_string, iron, gold, gems, exp;
        
        if(!(reader_ss >> cur_room >> raw_string >> iron >> gold >> gems >> exp)) {
             throw std::runtime_error(line);
        }

        if(check_is_digit(cur_room) && check_is_digit(iron) && check_is_digit(gold) && check_is_digit(gems) && check_is_digit(exp) == false) {
            throw std::runtime_error(line);
        }

        int i_cur_room, i_iron, i_gold, i_gems, i_exp;
        try {
            i_cur_room = std::stoi(cur_room);
            i_iron = iron.empty()? 0 : std::stoi(iron);
            i_gold = gold.empty()? 0 : std::stoi(gold);
            i_gems = gems.empty() ? 0 : std::stoi(gems);
            i_exp = exp.empty() ? 0 : std::stoi(exp);
        }
        catch (const std::exception& e) {
            throw std::runtime_error(line);
        }
        if(Validate(0, room_count, i_cur_room) == false) {
            throw std::runtime_error(line);
        }
        if(Validate(0, 255, i_iron) == false) {
            throw std::runtime_error(line);
        }
        if(Validate(0, 255, i_gold) == false) {
            throw std::runtime_error(line);
        }
        if(Validate(0, 255, i_gems) == false) {
            throw std::runtime_error(line);
        }
        if(Validate(0, 255, i_exp) == false) {
            throw std::runtime_error(line);
        }
        if(std::any_of(raw_string.begin(), raw_string.end(), [](char c){ return !std::isdigit(c) && c != ','; })) {
            throw std::runtime_error(line);
        }

        auto near_rooms = SplitByDelimiter(raw_string, ',');
        if (!near_rooms) {
            throw std::runtime_error(line);
        }
        for(int n_room : near_rooms.value()) {
            if(Validate(0, room_count, n_room) == false) {
                throw std::runtime_error(line);
            }
        }
        dungeon.resources_in_room[i_cur_room][Resource::Iron] = i_iron;
        dungeon.resources_in_room[i_cur_room][Resource::Gold] = i_gold;
        dungeon.resources_in_room[i_cur_room][Resource::Gems] = i_gems;
        dungeon.resources_in_room[i_cur_room][Resource::Exp] = i_exp;

        for(int n_room : near_rooms.value()) {
            dungeon.room_graph[i_cur_room].push_back(n_room);
            dungeon.room_graph[n_room].push_back(i_cur_room);
        }
    }
    std::getline(in, line);
    std::stringstream reader_ss(line);
    int food;
    reader_ss >> food;
    if(Validate(2, 255, food) == false) {
        throw std::runtime_error(line);
    }
    dungeon.food = food;
    Resource double_cost;
    std::string double_cost_str;
    reader_ss >> double_cost_str;
    if(double_cost_str == "iron") double_cost = Resource::Iron;
    else if(double_cost_str == "gold") double_cost = Resource::Gold;
    else if(double_cost_str == "gems") double_cost = Resource::Gems;
    else if(double_cost_str == "exp") double_cost = Resource::Exp;
    else throw std::runtime_error(line);
    dungeon.m_double_cost = double_cost;
    return dungeon;
}

std::optional<std::vector<int>> Reader::SplitByDelimiter(std::string splittable, char delimiter) {
    std::stringstream ss(splittable);
    std::string t;
    std::vector<int> near_rooms;
    
    while (getline(ss, t, delimiter)) {
        if(t.empty()) continue;
        near_rooms.push_back(std::stoi(t));
    }

    return near_rooms;
}

bool Reader::Validate(int min, int max, int value) {
    if(value < min || value > max) {
        return false;
    }
    return true;
}