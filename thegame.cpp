#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <algorithm>


const std::string CACHE("word_cache");
const std::size_t CACHE_SIZE = 500;

const std::string MOVIES = "movie_titles.txt";




std::vector<std::string> loadList(const std::string& filename) {
    std::vector<std::string> words;
    std::ifstream file;
    file.open(filename.c_str());
    while(file) {
        std::string line;
        file >> line;
        words.push_back(line);
    }
    file.close();
    return words;
}

std::vector<std::string> loadCache(const std::string& option) {
    if(option == "m")
        return std::vector<std::string>();
    return loadList(CACHE);
}

std::string normalize(const std::string& str) {
    std::string res(str);
    auto it = std::copy_if(str.begin(), str.end(), res.begin(), [](char c){ return !std::isspace(c); });
    res.resize(it - res.begin());

    std::transform(res.begin(), res.end(), res.begin(),

        [](unsigned char c){ return (!std::isalpha(c)) ? ' ' : std::tolower(c); });
    return res;
}



void split(std::vector<std::string>& out, const std::string& in) {
    if(in.empty())
        return;

    auto it1 = in.begin();
    auto it2 = in.begin();
    while(it1 != in.end()) {
        while(it2 != in.end() && !std::isspace(*++it2)) {}
        std::string word(std::string(it1, it2));
        out.push_back(word);
        if(it2 != in.end())
            ++it2;
        it1 = it2;
    }
}

class WordList
{
public:
    WordList(const std::string& filename)
        : m_filename(filename)
        , m_list(loadList(filename)) {
    }
    WordList(const std::string& filename, const std::vector<std::string>& list)
        : m_filename(filename)
        , m_list(list) {
    }

    bool isIn(const std::string& item) const {
        auto iCompare = [](const std::string& a, const std::string& b){
            return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                              [](char c1, char c2){ return std::toupper(c1) == std::toupper(c2); });
        };

        auto str = normalize(item);
        return std::find_if(m_list.begin(), m_list.end(),
                            [&](const auto& a){ return iCompare(a, str); })
                != m_list.end();
    }

    void add(const std::string& item) {
        m_list.push_back(item);
        resize();
        dump(m_list);
    }

    void dump(const std::vector<std::string>& list) {
        std::ofstream file;
        file.open(m_filename, std::ios_base::trunc);
        for(const auto& line : list)
            file << line << std::endl;
        file.close();
    }

    void resize() {
        if(m_list.size() > CACHE_SIZE) {
            decltype(m_list) tmp(m_list.begin() + (m_list.size() - CACHE_SIZE), m_list.end());
            m_list.swap(tmp);
        }
    }

    void extend(const WordList& other) {
    	std::set<std::string> unq;
    	for(const auto& item : other.m_list) {
            std::vector<std::string> tokens;
            split(tokens, normalize(item));
            unq.insert(tokens.begin(), tokens.end());
    	}
    	unq.insert(m_list.begin(), m_list.end());
    	m_list.clear();
    	m_list.insert(m_list.end(), unq.begin(),unq.end());
    }

    std::string pop() {
        if(m_list.empty())
            throw std::exception();
        unsigned en = m_list.size() - 1;
        std::mt19937 gen{m_rd()};
        std::uniform_int_distribution<unsigned> dist{ 0, en };
        unsigned selection = dist(gen);
        auto item = m_list[selection];
        m_list.erase(m_list.begin() + selection);
        return item;
    }
private:
    std::string m_filename;
    std::vector<std::string> m_list;
    std::random_device m_rd;
};

std::string fileForChoice(const std::string& choice) {
    if(choice == "c") {
        std::cout << "Game selected" << std::endl;
        return "common.txt";
    }
    if(choice == "m") {
        std::cout << "movies selected" << std::endl;
        return MOVIES;
    }
    if(choice == "x") {
        std::cout << "Tudor's kamera word list selected" << std::endl;
        return "tudors_word_list.txt";
    }
    if(choice == "Y0") {
    	WordList cache(CACHE, loadCache(choice));
    	cache.dump({});
    }
    if(choice == "p") {
        std::cout << "chosen corpus" << std::endl;
        return "text.txt";
    }

    std::cout << "Super badass ultimate triple dragon mega list" << std::endl;
    return "ultimate_turbo_swat_list.txt";
}


int main()
{
    std::cout << "Choose 'm' for movie titles, "
          "'x' for The Game with Tudor's extended kamera dictionary"
          "'c' for The Game with 800 common words"
          ", press enter for The Game with the Extreme explosion deathwave list" << std::endl;
    std::string option;
    getline(std::cin, option);

    WordList wordList(fileForChoice(option));
    WordList cache(CACHE, loadCache(option));
    if(option == "5") {
    	std::cout << "extended..." << std::endl;
		wordList.extend(WordList("movie_titles.txt"));
    }
    else if(option == "6") {
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
        std::cout << "    " << word << "     " << cnt << std::endl;
    }

    return 0;
}
