#pragma once

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
const std::size_t WORDS_IN_RESULT_SET = 10;

const std::string MOVIES = "movie_titles.txt";


std::string fileForChoice(const std::string& choice);

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

WordVec split(const std::string& line)
{
    WordVec w;
    split(w, line);
    return w;
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

        removeLeadingSpaceFromWords();
    }

    WordVec getWords() const {
        return words;
    }
private:
    void removeLeadingSpaceFromWords() {
        for(auto& word : words)
            removeLeadingSpaces(word);
    }

    void removeLeadingSpaces() {
        removeLeadingSpaces(line);
    }

    void removeLeadingSpaces(std::string& str) {
        while(!str.empty() && std::isspace(str[0]))
            str.erase(0, 1);
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
    WordKing(const std::string& option)
            : wordList(fileForChoice(option))
            , cache(CACHE, loadCache(option))
    {}

    WordVec gimme() {
        auto word(wordList.pop());
        while(cache.isIn(word)) {
            word = wordList.pop();
        }
        currentWords.push_back(word);
        cache.add(word);
        //resizeCurrentWords();
        return currentWords;
    }

private:
    void resizeCurrentWords() {
        auto offset = std::max(currentWords.size(), WORDS_IN_RESULT_SET) - WORDS_IN_RESULT_SET;
        WordVec swapper(currentWords.begin() + offset, currentWords.end());
        currentWords.swap(swapper);
    }

    WordList wordList;
    WordList cache;
    WordVec currentWords;
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

