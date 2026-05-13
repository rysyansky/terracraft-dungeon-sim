#### Environment: 
- OS: Arch Linux 6.9.13
- Compiler: GCC
- Standart: C++20
- Build system: CMake

Запуск осуществляется как ```app <input_file>```

IBotStrategy используется как интерфейс для удобной смены стратегий симуляции прохода бота по подемелью. AliceCrawler - реализованная стратегия прохода по подземелью, выполненная в соответствии с "Алгоритмом Алисы" из задания.
```mermaid
classDiagram
class IBotStrategy {
  <<interface>>
  + virtual void Initialize(int start_room, int initial_food)  
  + virtual BotAction DecideNextStep(const BotState& state)
  + virtual void Reset()
}
class GameSimulator {
  - IBotStrategy &bot_strategy
}
class AliceCrawler {
  + void Initialize(int start_room, int initial_food)  
  + BotAction DecideNextStep(const BotState& state)
  + void Reset()
}

IBotStrategy <|-- AliceCrawler
GameSimulator --* IBotStrategy
```
