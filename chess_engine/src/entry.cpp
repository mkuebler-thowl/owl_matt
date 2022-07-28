#pragma once

#include <iostream>
#include <vector>

#include "Position.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"

auto main() -> int
{
	auto engine = matt::ChessEngine();
	auto pos = matt::Position();

	// Testen
	//engine.searchMove(pos, matt::PLAYER_WHITE, 4, matt::FT_ALPHA_BETA | matt::FT_NESTED | matt::FT_SORT, false);
	//pos = matt::FENParser::fenToPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	//auto str = matt::FENParser::positionToFen(pos);
	//pos = matt::FENParser::fenToPosition(str);


	std::cout << "exit" << std::endl;
}