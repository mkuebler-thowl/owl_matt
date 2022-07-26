#pragma once

#include <iostream>

#include "Position.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"

auto main() -> int
{
	auto engine = matt::ChessEngine();
	auto pos = matt::Position();

	//engine.searchMove(pos, matt::PLAYER_WHITE, 4, matt::FT_ALPHA_BETA | matt::FT_NESTED | matt::FT_SORT, false);

	std::cout << "exit" << std::endl;
}