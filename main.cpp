#include "AliceCrawler.hpp"
#include "GameSimulator.hpp"
#include "IBotStrategy.hpp"
#include "Reader.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>


int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <dungeon_file>" << std::endl;
        return 1;
    }

    std::ofstream result("result.txt");
    if(!result) {
        std::cout << "result.txt did not open!\n";
        return 1;
    }
    Reader reader(argv[1]);
    Dungeon dungeon;
    try {
        dungeon = reader.GetDungeon();
    } catch (const std::exception& e) {
        result << e.what() << "\n";
        result.close();
        return 1;
    }

    std::unique_ptr<IBotStrategy> bot_strategy = std::make_unique<AliceCrawler>();
    GameSimulator simulator(dungeon, *bot_strategy);

    simulator.Run();
    std::vector<std::string> log = simulator.GetLog();
    for (const std::string &entry : log) {
        result << entry << "\n";
    }
    return 0;
}