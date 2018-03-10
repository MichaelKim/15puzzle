#include "../include/InputParser.h"

#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::vector<std::string>> InputParser::tokens{};

void InputParser::parse(int argc, const char* argv[]) {
    if (argc > 1 && argv[1][0] == '-') {
        std::string option;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {  // new option
                option = argv[i];
                tokens[option] = std::vector<std::string>();
            }
            else {  // option argument
                tokens[option].push_back(std::string(argv[i]));
            }
        }
    }
}

bool InputParser::showHelp() {
    return optionExists("-h") || optionExists("--help");
}

bool InputParser::databaseExists() {
    return optionExists("-d") || optionExists("--database");
}

bool InputParser::boardExists() {
    return optionExists("-b") || optionExists("--board");
}

bool InputParser::showInteractive() {
    return optionExists("-i") || optionExists("--interactive");
}

bool InputParser::runParallel() {
    return optionExists("-p") || optionExists("--parallel");
}

std::string InputParser::getDatabase() {
    auto args = getMultipleArgs({"-d", "--database"});
    if (args.size() > 0) {
        return args[0];
    }
    return "";
}

std::string InputParser::getBoard() {
    auto args = getMultipleArgs({"-b", "--board"});
    if (args.size() > 0) {
        return args[0];
    }
    return "";
}

bool InputParser::optionExists(std::string option) {
    return tokens.find(option) != tokens.end();
}

std::vector<std::string> InputParser::getArgs(std::string option) {
    return tokens[option];
}

std::vector<std::string> InputParser::getMultipleArgs(
    std::vector<std::string> options) {
    std::vector<std::string> args;
    for (auto option : options) {
        auto nextArgs = getArgs(option);
        args.insert(args.end(), nextArgs.begin(), nextArgs.end());
    }
    return args;
}
