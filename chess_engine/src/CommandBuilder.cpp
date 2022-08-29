#include "CommandBuilder.hpp"
#include <array>
#include <regex>

namespace owl
{
    /**
     * Regex-Liste zur Filterung der UCI-Befehle.
     */
    const std::array<const std::string, 13U> COMMAND_VALIDATION_MAP = {
        "^uci$",
        "^debug (on|off)$",
        "^isready$",
        "^setoption name (.*?)(?:\\svalue (.+))?$",
        "^position (startpos|(?:[knprqbKNPRQB1-8\\/]+ [wb] [KQkq\\-]{1,4} [a-h1-8\\-]{1,2} \\d+ \\d+)) moves ((?:[a-h0-8xqbrnQBRN\\+\\-]{4,7}\\s?)*)",
        "^quit$",
        "^go(?: (?:(infinite)|(searchmoves (?:[a-h0-8x\\+]{4,6}\\s?)+)|(ponder)|(wtime \\d+)|(btime \\d+)|(winc -?\\d+)|(binc -?\\d+)|(movestogo \\d+)|(depth \\d+)|(nodes \\d+)|(mate \\d+)|(movetime \\d+)|(infinite)))+$",
        "^stop$",
        "^ponderhit$",
        "^register (later)|(?:(name .*(?= code))? (code .*)?)$",
        "^evaluate$",
        "^getvalidmoves$",
        "^ucinewgame$"
    };
    Command CommandBuilder::build(const std::string& command_str)
    {
        std::smatch match;

        for (size_t i = 0; i < COMMAND_VALIDATION_MAP.size(); i++) {
            if (std::regex_search(command_str, match, std::regex(COMMAND_VALIDATION_MAP[i])))
            {
                std::vector<std::string> args;
                for (size_t i = 1; i < match.size(); i++) {
                    if (match.str(i).empty()) continue;

                    args.push_back(match.str(i));
                }

                return Command((Command::Type)i, args);
            }
        }

        // the command is not in the list of valid commands
        return Command(Command::Type::INVALID, {});
    }
}