#include "FENParser.hpp"

namespace matt {

    Position FENParser::fenToPosition(const std::string& fen)
    {
        // TODO:
        return Position();
    }
    std::string FENParser::positionToFen(const Position& pos)
    {
        // TODO:
        return std::string();
    }

    Move FENParser::stringToMove(const std::string& algebraicNotation)
    {
        Move move;

        // capture abfragen:
        move.capture = algebraicNotation.find("x") != std::string::npos;

        // promotion abfragen:
        move.promotion = algebraicNotation.size() == 6 ? algebraicNotation[5] : 0;
        
        // position setzen
        move.startX = algebraicNotation[0] - 'a';
        move.startY = 7 - algebraicNotation[1] - '1';
        move.targetX = algebraicNotation[3] - 'a';
        move.targetY = 7 - algebraicNotation[4] - '1';

        return move;
    }

    std::string FENParser::moveToString(const Move& move)
    {
        std::string out = "";

        // start
        out += move.startX + 'a';
        out += '8' - move.startY;

        // capture
        out += move.capture ? 'x' : '-';

        // target
        out += move.targetX + 'a';
        out += '8' - move.targetY;

        // promotion
        out += move.promotion;

        return out;
    }
}