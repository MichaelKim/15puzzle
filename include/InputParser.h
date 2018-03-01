#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <unordered_map>
#include <vector>

class InputParser {
    public:
        InputParser(int argc, const char* argv[]) {
            if (argc > 1 && argv[1][0] == '-') {
                std::string option = argv[1];
                tokens[option] = std::vector<std::string>();
                for (int i = 2; i < argc; i++) {
                    if (argv[i][0] == '-') { // new option
                        option = argv[i];
                    }
                    else { // option argument
                        tokens[option].push_back(std::string(argv[i]));
                    }
                }
            }
        }
        ~InputParser() {}

        bool optionExists(std::string option) {
            return tokens.find(option) != tokens.end();
        }

        std::vector<std::string> getArgs(std::string option) {
            return tokens[option];
        }
    private:
        std::unordered_map<std::string, std::vector<std::string>> tokens;
};

#endif
