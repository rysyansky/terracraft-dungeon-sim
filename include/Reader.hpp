#pragma once
#include "Dungeon.hpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>



class Reader {
public:
    Reader(const std::string&);
    ~Reader();

    Dungeon GetDungeon();
private:
    std::ifstream in;
    Dungeon dungeon;

    std::optional<std::vector<int>> SplitByDelimiter (std::string splittable, char delimeter);
    bool Validate(int min, int max, int value);
};