#pragma once

#include "defines.hpp"
#include "UniversalChessInterface.hpp"

#include "ChessUtility.hpp"

auto main() -> owl::INT32
{
	// UCI starten:
	owl::UniversalChessInterface uci;
	uci.start();

	//auto pos = owl::ChessUtility::fenToPosition(owl::STARTPOS_FEN);

	//auto engine = owl::ChessEngine();
	//engine.setPosition(pos);

	//auto result = engine.searchMove(engine.getPlayer(), 0, owl::FT_STANDARD, false);

	//std::cout << owl::ChessUtility::moveToString(result.first);
	//owl::PRINT_EVALUATION_VALUE(result.second);

	//std::cout << "exit" << std::endl;

}