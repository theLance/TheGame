#include "thegame.hpp"

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
    while(select != "q") {
        getline(std::cin, select);
        auto word(wordList.pop());
        while(cache.isIn(word)) {
            word = wordList.pop();
        }
        if(select == ".")
            cnt++;
        cache.add(word);
        std::cout << "    [" << word << "]     " << cnt << std::endl;
    }

    return 0;
}
