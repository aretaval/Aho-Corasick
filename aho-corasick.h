#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <queue>

class AhoCorasick {
    struct Node {
        std::vector<Node*> children;
        Node* suffix{nullptr};
        Node* dictionarySuffix{nullptr};
        bool isTerminal{false};
        std::string state;

        Node() = default;
        explicit Node(size_t size) {
            children.resize(size, nullptr);
        }

        void printNode() const;

        ~Node() = default;
    };

public:
    explicit AhoCorasick(const std::string &words);

    void printAutomata() const ;
    void search(const std::string &text, bool protocol);

    ~AhoCorasick();

private:
    Node* root;

    std::map<char, size_t> alphabet;
    size_t alphabetSize{0};
    std::set<std::string> dictionary;
    std::map<std::string, size_t> foundWords;

    void setDictionary(const std::string& words);
    void setAlphabet();
    void addWord(const std::string& word);

    void setSuffixLinks();
    void setDictionarySuffixLinks();
    void createAutomata();

    void printFoundWords() const;
};


// ----AhoCorasick::Node----

void AhoCorasick::Node::printNode() const {
    if (!state.empty()) {
        std::cout << state << "\n";
    } else {
        std::cout << "epsilon\n";
    }
    std::cout << "is terminal: ";
    if (isTerminal) {
        std::cout << "yes\n";
    } else {
        std::cout << "no\n";
    }

    std::cout << "children:";
    std::string sep;
    for (auto * child : children) {
        if (child != nullptr) {
            std::cout << sep << " " << child->state;
            sep = ",";
        }
    }
    std::cout << "\n";

    std::cout << "suffix: ";

    if (suffix != nullptr) {
        if (!suffix->state.empty()) {
            std::cout << suffix->state << "\n";
        } else {
            std::cout << "epsilon\n";
        }
    } else {
        std::cout << "-\n";
    }

    std::cout << "dictionary suffix: ";
    if (dictionarySuffix != nullptr) {
        std::cout << dictionarySuffix->state << "\n";
    } else {
        std::cout << "-\n";
    }
    std::cout << "\n";
}



// ----Aho-Corasick----

AhoCorasick::AhoCorasick(const std::string &words) : root(new Node) {
    setDictionary(words);

    setAlphabet();

    for (const auto& word : dictionary) {
        addWord(word);
    }

    createAutomata();
}

void AhoCorasick::setDictionary(const std::string &words) {
    std::istringstream ss(words);

    std::string curWord;
    while (ss >> curWord) {
        dictionary.insert(curWord);
    }
}

void AhoCorasick::createAutomata() {
    setSuffixLinks();
    setDictionarySuffixLinks();
}

void AhoCorasick::setAlphabet() {
    size_t i = 0;
    for (const auto& word : dictionary) {
        for (char c : word) {
            if (alphabet.find(c) == alphabet.end()) {
                alphabet[c] = i++;
            }
        }
    }

    alphabetSize = i;

    root->children.resize(alphabetSize, nullptr);
}

void AhoCorasick::addWord(const std::string &word) {
    Node* current = root;
    for (char c : word) {
        if (current->children[alphabet[c]] == nullptr) {
            auto * newNode = new Node(alphabetSize);
            current->children[alphabet[c]] = newNode;
            newNode->state = current->state + c;

            current = newNode;
        } else {
            current = current->children[alphabet[c]];
        }
    }

    current->isTerminal = true;

    foundWords[word] = 0;
}

void AhoCorasick::setSuffixLinks() {
    root->suffix = root;

    std::queue<Node*> nodes;

    for (auto * child : root->children) {
        if (child != nullptr) {
            child->suffix = root;

            nodes.push(child);
        }
    }

    while (!nodes.empty()) {
        auto * currentFirst = nodes.front();
        nodes.pop();

        for (auto * child : currentFirst->children) {
            if (child != nullptr) {
                char lastLetter = child->state[child->state.length() - 1];

                auto * current = currentFirst;
                while (current->suffix->children[alphabet[lastLetter]] == nullptr
                       && current != root) {
                    current = current->suffix;
                }
                if (current->suffix->children[alphabet[lastLetter]] != nullptr) {
                    child->suffix = current->suffix->children[alphabet[lastLetter]];
                } else {
                    child->suffix = root;
                }

                nodes.push(child);
            }
        }
    }
}

void AhoCorasick::setDictionarySuffixLinks() {
    std::queue<Node*> nodes;
    nodes.push(root);

    while (!nodes.empty()) {
        auto * currentFirst = nodes.front();
        nodes.pop();

        for (auto * child : currentFirst->children) {
            if (child != nullptr) {
                auto * current = child;

                while (!current->suffix->isTerminal && current != root) {
                    current = current->suffix;
                }
                if (current->suffix->isTerminal) {
                    child->dictionarySuffix = current->suffix;
                }

                nodes.push(child);
            }
        }
    }
}

void AhoCorasick::printFoundWords() const {
    for (const auto& elem : foundWords) {
        std::cout << elem.first << ": " << elem.second << "\n";
    }
    std::cout << "\n";
}

void AhoCorasick::printAutomata() const {
    std::queue<Node*> nodes;
    nodes.push(root);

    while (!nodes.empty()) {
        auto * current = nodes.front();
        nodes.pop();

        current->printNode();

        for (auto * child : current->children) {
            if (child != nullptr) {
                nodes.push(child);
            }
        }
    }
}

void AhoCorasick::search(const std::string &text, bool protocol) {
    foundWords.clear();

    Node* current = root;

    for (size_t i = 0; i < text.length(); ++i) {
        if (alphabet.find(text[i]) == alphabet.end()) {
            current = root;
        } else {
            while (current->children[alphabet[text[i]]] == nullptr && current !=  root) {
                current = current->suffix;
            }

            if (current->children[alphabet[text[i]]] != nullptr) {
                current = current->children[alphabet[text[i]]];
            } else {
                current = root;
            }

            if (current->isTerminal) {
                ++foundWords[current->state];
            }


            auto * next = current;
            while (next->dictionarySuffix != nullptr) {
                ++foundWords[next->dictionarySuffix->state];
                next = next->dictionarySuffix;
            }
        }

        if (protocol) {
            std::cout << "current prefix: " << text.substr(0, i + 1) << "\n";
            std::cout << "current state: ";
            if (!current->state.empty()) {
                std::cout << current->state << "\n";
            } else {
                std::cout << "epsilon\n";
            }
            printFoundWords();

            std::cout << "\n";
        }

    }
    printFoundWords();
}

AhoCorasick::~AhoCorasick() {
    std::queue<Node*> nodes;

    nodes.push(root);

    while (!nodes.empty()) {
        auto * current = nodes.front();
        nodes.pop();

        for (auto * child : current->children) {
            if (child != nullptr) {
                nodes.push(child);
            }
        }

        delete current;
    }
}
