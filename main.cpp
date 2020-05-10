#include "thegame.hpp"

#include <chrono>
#include <thread>
#include <conio.h>
#include <functional>

enum Time {
    TOTAL_TIME = 90,
    TURN_TIME = 5
};

bool TURN_BASED = false;

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

struct ThreadLauncher
{
    ThreadLauncher(std::function<void()> func) {
        std::thread timer([&]() {
            func();
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

std::string readOption() {
    std::string option;
    getline(std::cin, option);
    return option;
}

std::string menu() {
    std::cout << R"SQL(Choose 'm' for movie titles, 
                 'x' for The Game with Tudor's extended kamera dictionary,
                 'c' for The Game with 800 common words,
                 'p' for Extreme explosion deathwave list,
                 press enter for The Game with the total badass random text corpus shit;
                 'q' to quit
                 Commemorating our good friend Tudor, who strays far from ours to the dark pits of Europe :))SQL"
                 << std::endl;
//    if(option == "6") {
//        std::cout << "extended2..." << std::endl;
//        wordList.extend(WordList("other.txt"));
//    }
    return readOption();
}


int main()
{
    bool continueGame = true;
    do {
        /// kicsit gany, de most mit lehet tenni. TODO: De-gany this crap
        try {
            WordKing king(menu());
            WordVec words;
            Timer timer;
            std::function<void()> timerAction;
            if(TURN_BASED) {
                timerAction = []() {
                    do {
                        std::this_thread::sleep_for(std::chrono::seconds(Time::TURN_TIME));
                        std::cout << "+";
                    } while(true);
                };
            } else {
                timerAction = [&]() {
                    std::this_thread::sleep_for(std::chrono::seconds(Time::TOTAL_TIME));
                    king.endIt();
                };
            }
            ThreadLauncher timerThingy(timerAction);
            double theTime = timer.remaining();
            while(true) {
                auto words = king.gimme();
                theTime = timer.remaining();
                printScreen(words, theTime);
                char c = getch();
                if(c == 'q')
                    break;
            }
        } catch(WordKing::GameOver&) {
        }
        std::cout << "-------- GAME OVER --------\n Continue? (enter for 'yes', everything else for no)" << std::endl;
        continueGame = readOption().empty();
    } while(continueGame);

    return 0;
}
