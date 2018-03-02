#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
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

        bool showHelp() {
            return optionExists("-h") || optionExists("--help");
        }

        bool databaseExists() {
            return optionExists("-d") || optionExists("--database");
        }

        bool boardExists() {
            return optionExists("-b") || optionExists("--board");
        }

        std::string getDatabase() {
            auto args = getMultipleArgs({"-d", "--database"});
            if (args.size() > 0) {
                return args[0];
            }
            return "";
        }

        std::string getBoard() {
            auto args = getMultipleArgs({"-b", "--board"});
            if (args.size() > 0) {
                return args[0];
            }
            return "";
        }

    private:
        std::unordered_map<std::string, std::vector<std::string>> tokens;

        bool optionExists(std::string option) {
            return tokens.find(option) != tokens.end();
        }

        std::vector<std::string> getArgs(std::string option) {
            return tokens[option];
        }

        std::vector<std::string> getMultipleArgs(std::vector<std::string> options) {
            std::vector<std::string> args;
            for (auto option: options) {
                auto nextArgs = getArgs(option);
                args.insert(args.end(), nextArgs.begin(), nextArgs.end());
            }
            return args;
        }
};

#endif
