#include "thegame.hpp"

#include <chrono>
#include <thread>
#include <conio.h>

long long TOTAL_TIME = 90;

class Timer
{
public:
    Timer() : startTime(std::chrono::steady_clock::now()) {}
    auto elapsed() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    }

    bool isOver() const {
        std::cout << "elapsed=" << elapsed() << std::endl;
        return elapsed() >= TOTAL_TIME;
    }

    auto remaining() const {
        return (TOTAL_TIME - elapsed());
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
            std::this_thread::sleep_for(std::chrono::seconds(TOTAL_TIME));
            throw GameOver();
        });
        timer.detach();
    }
private:
};

void printScreen(const WordVec& current, double time)
{
    for(int i = 50; i != 0; i--)
        std::cout << std::endl;
    for(auto&& w : current) {
        std::cout << "\n   [" << w << "]     ";
    }
    std::cout << time;
    std::cout << std::endl;
    std::cout << std::endl;

}


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

    WordVec words;
    Timer timer;
    DeathTimer deathCreeping;
    double theTime = timer.remaining();
    while(true) {
        auto word(wordList.pop());
        while(cache.isIn(word)) {
            word = wordList.pop();
        }
        words.push_back(word);
        cache.add(word);
        theTime = timer.remaining();
        printScreen(words, theTime);
        char c = getch();
        if(c == 'q')
           break;
    }
    return 0;
}
