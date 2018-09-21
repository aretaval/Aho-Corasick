#include "aho-corasick.h"

int main() {
    std::cout << "enter words: \n";
    std::string wordsLine;
    std::getline(std::cin, wordsLine);

    AhoCorasick ac(wordsLine);

    std::cout << "\n";
    std::cout << "p - print automata\n";
    std::cout << "s - search words\n";
    std::cout << "sp - search with protocol\n";
    std::cout << "e - end\n";
    std::cout << "\n";

    std::string option;
    std::cin >> option;

    while (option != "e") {
        if (option == "p") {
            ac.printAutomata();
        } else if (option == "s") {
            std::string text;

            std::getline(std::cin, text);

            ac.search(text, false);
        } else if (option == "sp") {
            std::string text;

            std::getline(std::cin, text);

            ac.search(text, true);
        }

        std::cin >> option;
    }

    return 0;
}