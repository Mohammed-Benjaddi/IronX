

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

bool is_operator(const std::string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

bool spacingValidation(std::string input) {
    bool lastWasChar = false;

    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] != ' ') {
            if (lastWasChar) {
                return false;
            }
            lastWasChar = true;
        } else {
            lastWasChar = false;
        }
    }
    return true;
}

void process_rpn(const std::string& expression) {
    std::istringstream stream(expression);
    std::string token;
    std::vector<int> stack;

    while (stream >> token) {
        if (is_operator(token)) {
            if (stack.size() < 2) {
                std::cerr << "Error: Not enough operands for operation: " << token << std::endl;
                return;
            }

            int b = stack.back(); stack.pop_back();
            int a = stack.back(); stack.pop_back();

            if (token == "+") stack.push_back(a + b);
            else if (token == "-") stack.push_back(a - b);
            else if (token == "*") stack.push_back(a * b);
            else if (token == "/") {
                if (b == 0) {
                    std::cerr << "Error: Division by zero" << std::endl;
                    return;
                }
                stack.push_back(a / b);
            }
        } else {
            try {
                stack.push_back(std::stoi(token));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid token: " << token << std::endl;
                return;
            }
        }
    }

    if (stack.size() != 1) {
        std::cerr << "Error: Too many operands or insufficient operations" << std::endl;
    } else {
        std::cout << "Result: " << stack.back() << std::endl;
    }
}

int main() {
    std::ifstream file("rpn_invalid_cases.txt");
    std::string line;

    while (std::getline(file, line)) {
        std::cout << "Processing: " << line << std::endl;
        if (!spacingValidation(line)) {
            std::cerr << "Error: Invalid spacing in expression: " << line << std::endl;
            continue;
        }
        process_rpn(line);
    }

    return 0;
}