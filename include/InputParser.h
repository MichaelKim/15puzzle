#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
#include <unordered_map>
#include <vector>

class InputParser {
public:
    static void parse(int argc, const char* argv[]);
    static bool showHelp();
    static bool databaseExists();
    static bool boardExists();
    static bool showInteractive();
    static bool runParallel();

    static std::string getDatabase();
    static std::string getBoard();

private:
    static std::unordered_map<std::string, std::vector<std::string>> tokens;

    InputParser() {}

    static bool optionExists(const std::string& option);
    static std::vector<std::string> getArgs(const std::string& option);
    static std::vector<std::string> getMultipleArgs(
        const std::vector<std::string>& options);
};

#endif
