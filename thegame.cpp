#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <algorithm>


typedef std::vector<std::string> WordVec;
typedef std::set<std::string> WordSet;
const std::string CACHE("word_cache");
const std::size_t CACHE_SIZE = 500;

const std::string MOVIES = "movie_titles.txt";


void split(WordVec& out, const std::string& in) {
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

class LineFormatter
{
public:
    LineFormatter(const std::string& l) : line(l)
    {
        removeLeadingSpaces();
        removeSpecialChars();
        removeMultipleSpaces();
        removeTrailingSpace();

        split(words, line);
    }

    WordVec getWords() const {
        return words;
    }
private:
    void removeLeadingSpaces() {
        while(!line.empty() && std::isspace(line[0]))
            line.erase(0, 1);
    }

    bool isAcceptableCharacter(char c) {
        static const std::string acceptable("'-_\"");
        return ( std::isalpha(c) || std::any_of(acceptable.begin(), acceptable.end(), [c](char k){ return k == c; }) );
    }

    void removeSpecialChars() {
        std::transform(line.begin(), line.end(), line.begin(),
                       [this](unsigned char c){ return (isAcceptableCharacter(c)) ? std::tolower(c) : ' '; });
    }

    void removeMultipleSpaces() {
        auto newEnd = std::unique(line.begin(), line.end(), [](char l, char r){ return std::isspace(l) && std::isspace(r);});
        line.erase(newEnd, line.end());
    }

    void removeTrailingSpace() {
        while(!line.empty() && std::isspace(line.back()))
            line.pop_back();
    }

    std::string line;
    WordVec words;
};


WordVec makeUnique(const WordVec& one, const WordVec& other)
{
    WordSet wset(one.begin(), one.end());
    wset.insert(other.begin(), other.end());
    return { wset.begin(), wset.end() };
}

WordVec loadList(const std::string& filename) {
    WordVec words;
    std::ifstream file;
    file.open(filename.c_str());
    while(file) {
        std::string line;
        file >> line;
        LineFormatter lf(line);
        auto newWords = lf.getWords();
        words.insert(words.end(), newWords.begin(), newWords.end());
    }
    file.close();

    // make unique
    auto uwords = makeUnique(words, {});
    words.swap(uwords);
    return words;
}

WordVec loadCache(const std::string& option) {
    if(option == "m")
        return std::vector<std::string>();
    return loadList(CACHE);
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
    WordList(const WordList& wl)
        : m_filename(wl.m_filename)
        , m_list(wl.m_list)
    {
    }

    bool isIn(const std::string& item) const {
        auto iCompare = [](const std::string& a, const std::string& b){
            return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                              [](char c1, char c2){ return std::toupper(c1) == std::toupper(c2); });
        };

        return std::find_if(m_list.begin(), m_list.end(),
                            [&](const auto& a){ return iCompare(a, item); })
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
    	WordSet unq;
    	for(const auto& item : other.m_list) {
            WordVec tokens;
            split(tokens, item);
            unq.insert(tokens.begin(), tokens.end());
    	}
    	unq.insert(m_list.begin(), m_list.end());
    	m_list.clear();
    	m_list.insert(m_list.end(), unq.begin(),unq.end());
    }

    std::string pop() {
        if(m_list.empty())
        {
            std::cout << "There are no words in the word list" << std::endl;
            throw std::exception();
        }
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
    WordVec m_list;
    std::random_device m_rd;
};

class WordKing
{
public:
    WordKing(WordList wlist)
        : wl(wlist)
        , current()
    {}

    WordVec gimme() {
        current.push_back(wl.pop());
        WordVec toSwapWith{current.begin() + 1, current.end()};
        current.swap(toSwapWith);
        return current;
    }

private:
    WordList wl;
    WordVec current;
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
          " will reeeeaaallly have to update this list...."
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
