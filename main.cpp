#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <stack>
#include <sstream>

struct Transition {
    int from_state;
    int to_state;
    std::string transition_name; 
    std::string pop_symbol; 
    std::vector<std::string> push_symbols;  
};

class FiniteAutomaton {
private:
    std::map<int, std::string> state_map;
    std::vector<Transition> transitions; 
    std::stack<std::string> stack;  
    int final_state; 

public:

    FiniteAutomaton() {
    addState("q0");
    stack.push("e"); 
}

    void addState(const std::string& state) {
        int state_index = state_map.size();
        state_map[state_index] = state;
    }

    void addTransition(int from_state, int to_state, const std::string& transition_name,
                       const std::string& pop_symbol, const std::vector<std::string>& push_symbols) {
        Transition new_transition = {from_state, to_state, transition_name, pop_symbol, push_symbols};
        transitions.push_back(new_transition);
    }

    void printStates() const {
        std::cout << "Состояния автомата:\n";
        for (const auto& state_pair : state_map) {
            std::cout << "Состояние " << state_pair.first << ": " << state_pair.second << std::endl;
        }
    }

    void printTransitions() const {
        std::cout << "Переходы автомата:\n";
        for (const auto& transition : transitions) {
            std::cout << "Из состояния " << transition.from_state << " в состояние " << transition.to_state 
                      << " по переходу: '" << transition.transition_name << "'"
                      << " c удалением '" << transition.pop_symbol << "' и добавлением ";
            for (const auto& sym : transition.push_symbols) {
                std::cout << "'" << sym << "' ";
            }
            std::cout << std::endl;
        }
    }

    bool checkTransitionFromState(int current_state, const std::string& transition_name) const {
        for (const auto& transition : transitions) {
            if (transition.from_state == current_state && transition.transition_name == transition_name) {
                return true; 
            }
        }
        return false; 
    }

    void processInput(const std::string& input) {
        int current_state = 0;
        for (char ch : input) {
            bool transition_found = false;
            for (const auto& transition : transitions) {
                if (transition.from_state == current_state && transition.transition_name == std::string(1, ch)) {
                    if ((transition.pop_symbol.empty() || transition.pop_symbol == "-" || (!stack.empty() && stack.top() == transition.pop_symbol))) {
                        if (!transition.pop_symbol.empty() && transition.pop_symbol != "-" && !stack.empty() && stack.top() == transition.pop_symbol) {
                            stack.pop();
                        }
                        for (const auto& sym : transition.push_symbols) {
                            if (sym != "-") {  
                                stack.push(sym);
                            }
                        }
                        current_state = transition.to_state;
                        transition_found = true;
//                        printStack();
                        break;
                    }
                }
            }
            if (!transition_found) {
                std::cout << "Ошибка перехода из состояния " << current_state << " по символу '" << ch << "'\n";
                return;
            }
        }

        bool empty_transition_found;
        do {
            empty_transition_found = false;
            for (const auto& transition : transitions) {
                if (transition.from_state == current_state && transition.transition_name == "-") {
                    if ((transition.pop_symbol.empty() || transition.pop_symbol == "-" || (!stack.empty() && stack.top() == transition.pop_symbol))) {
                        if (!transition.pop_symbol.empty() && transition.pop_symbol != "-" && !stack.empty() && stack.top() == transition.pop_symbol) {
                            stack.pop();
                        }
                        for (const auto& sym : transition.push_symbols) {
                            if (sym != "-") { 
                                stack.push(sym);
                            }
                        }
                        current_state = transition.to_state;
                        empty_transition_found = true;
//                        printStack();
                        break;
                    }
                }
            }
        } while (empty_transition_found);

        if (isFinalState(current_state)) {
            std::cout << "Достигнуто финальное состояние. Входная строка принята.\n";
        } else {
            std::cout << "Входная строка не принята. Текущее состояние не является финальным.\n";
        }
    }

    void printStack() {
        std::stack<std::string> temp_stack = stack; 
        std::cout << "Текущее содержимое стека: ";
        if (temp_stack.empty()) {
            std::cout << "Пусто";
        } else {
            while (!temp_stack.empty()) {
                std::cout << "'" << temp_stack.top() << "' ";
                temp_stack.pop();
            }
        }
        std::cout << std::endl;
    }

    void setFinalState(int state) {
        final_state = state;
    }

    bool isFinalState(int state) const {
        return state == final_state;
    }

    void printFinalState() const {
        std::cout << "Финальное состояние: " << final_state << std::endl;
    }

    void readTextFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            std::cout << std::endl << "Прочитана строка из файла: " << line << std::endl;
            processInput(line);
        }

        file.close();
    }
};

// Функция для чтения переходов из файла
void readTransitionsFromFile(const std::string& filename, FiniteAutomaton& automaton) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Ошибка открытия файла " << filename << std::endl;
        return;
    }

    int finalStateId;
    if (!(inputFile >> finalStateId)) {
        std::cerr << "Не удалось прочитать идентификатор финального состояния." << std::endl;
        inputFile.close();
        return;
    }

    automaton.setFinalState(finalStateId);

    std::cout << "Установлено финальное состояние: " << finalStateId << std::endl;

    int from_state, to_state;
    std::string transition_name, pop_symbol, push_symbol;
    while (inputFile >> from_state >> to_state >> transition_name >> pop_symbol) {
        std::vector<std::string> push_symbols;
        while (inputFile >> push_symbol && push_symbol != ";") {
            push_symbols.push_back(push_symbol);
        }
        automaton.addTransition(from_state, to_state, transition_name, pop_symbol, push_symbols);
    }

    inputFile.close();
}

int main() {
    FiniteAutomaton automaton;

    readTransitionsFromFile("automaton.txt", automaton);

    automaton.printTransitions();

    std::string text = "text.txt"; 
    automaton.readTextFromFile(text);

    return 0;
}
