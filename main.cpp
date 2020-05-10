#include "thegame.hpp"

#include <chrono>
#include <thread>
#include <conio.h>
#include <functional>

enum Time {
    TOTAL_TIME = 90,
    TURN_TIME = 5
};

bool TURN_BASED = true;

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

struct GameOver{};

static void csicsika(long long time) {
    std::this_thread::sleep_for(std::chrono::seconds(time));
}

struct TickTimer
{
    TickTimer(std::function<void()> func, Time roundTime, bool restartable) {
        std::thread timer([&]() {
            do {
                //std::this_thread::sleep_for(std::chrono::seconds(roundTime));
                csicsika(roundTime);
                func();
            } while(restartable);
        });
        timer.detach();
    }
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
                 " will reeeeaaallly have to update this list...."
                 ", press enter for The Game with the Extreme explosion deathwave list" << std::endl;
    std::string option;
    getline(std::cin, option);
//    if(option == "6") {
//        std::cout << "extended2..." << std::endl;
//        wordList.extend(WordList("other.txt"));
//    }

    WordVec words;
    Timer timer;
    std::function<void()> timerAction;
    Time toDie = Time::TOTAL_TIME;
    if(TURN_BASED) {
        toDie = Time::TURN_TIME;
        timerAction = [](){ std::cout << "+"; };
    } else {
        timerAction = [](){ throw GameOver(); };
    }
    TickTimer timerThingy(timerAction, toDie, TURN_BASED);
    WordKing king(option);
    double theTime = timer.remaining();
    while(true) {
        auto words = king.gimme();
        theTime = timer.remaining();
        printScreen(words, theTime);
        char c = getch();
        if(c == 'q')
           break;
    }
    return 0;
}
