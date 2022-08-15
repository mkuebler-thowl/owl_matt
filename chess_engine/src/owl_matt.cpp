#pragma once

#include <iostream>
#include <string>

#include "defines.hpp"
#include "UniversalChessInterface.hpp"
#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"
#include "FENParser.hpp"

auto testGetValidMoves() -> owl::VOID
{
	auto pos = owl::FENParser::fenToPosition("8/r5B1/PRk1B3/P1P5/3P3P/2P3P1/5K1P/R7 b - - 3 36");

	auto moves = owl::ChessValidation::getValidMoves(pos, pos.getPlayer());

	for (auto& move : moves)
	{
		std::cout << "{ " << move.startX << "-" << move.startY << " : " << move.targetX << "-" << move.targetY << ", "
			<< owl::FENParser::moveToString(move)
			<< ", not valid: " << move.isMoveInvalid()
			<< " }" << std::endl;
	}
}

auto testEngine() -> owl::VOID
{
	//auto pos = owl::FENParser::fenToPosition("r1b4r/pppk1Npp/5n2/8/2Bp4/Bn6/P4PPP/4R1K1 w - - 0 1");
	//auto engine = owl::ChessEngine();
	//engine.setPosition(pos);
	//auto best = engine.searchMove(pos.getPlayer(), 6, owl::FT_ALPHA_BETA | owl::FT_SRT_MVV_LVA | owl::FT_SRT_MATERIAL | owl::FT_SRT_KILLER);
}

auto main() -> owl::INT32
{
	// uci 
	//owl::UniversalChessInterface uci;
	//uci.start();

	std::cout << std::endl;
}
