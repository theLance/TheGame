#include "thegame.hpp"

#include <chrono>
#include <thread>

double TOTAL_TIME = 5;

class Timer
{
public:
    Timer() : startTime(std::chrono::steady_clock::now()) {}
    auto elapsed() {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    }

    bool isOver() {
        std::cout << "elapsed=" << elapsed() << std::endl;
        return elapsed() >= TOTAL_TIME;
    }
private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

class DeathTimer
{
public:
    struct GameOver{};
    DeathTimer() {
        std::thread timer([]() {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            throw GameOver();
        });
        timer.detach();
    }
private:
};


int main()
{
    std::cout << "Choose 'm' for movie titles, "
                 "'x' for The Game with Tudor's extended kamera dictionary"
                 "'c' for The Game with 800 common words"
                 " will reeeeaaallly have to up                                date this list...."
                 ", press enter for The Game with the Extreme explosion deathwave list" << std::endl;
    std::string option;
    getline(std::cin, option);

    WordList wordList(fileForChoice(option));
    WordList cache(CACHE, loadCache(option));
    if(option == "6") {
        std::cout << "extended2..." << std::endl;
        wordList.extend(WordList("other.txt"));
    }

    std::string select;
    int cnt = 0;
    //DeathTimer dt;
    while(select != "q") {
        auto word(wordList.pop());
        while(cache.isIn(word)) {
            word = wordList.pop();
        }
        if(select == ".") {
            cnt++;
        }
        cache.add(word);
        std::cout << "    [" << word << "]     " << cnt << std::endl;
        getline(std::cin, select);
    }
    return 0;
}
