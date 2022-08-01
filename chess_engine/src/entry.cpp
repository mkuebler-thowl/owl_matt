#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "Move.hpp"
#include "Position.hpp"
#include "ChessEngine.hpp"
#include "ChessEvaluation.hpp"
#include "FENParser.hpp"

// Test: Print move list
auto printMoves(std::vector<matt::Move> moves, const matt::Position& pos) -> void
{
	std::string out = "";

	int index = 0;

	for (auto move : moves)
	{
		out += "(";
		out += "{" + std::to_string(move.startX) + ", " + std::to_string(move.startY) + "} -> ";
		out += "{" + std::to_string(move.targetX) + ", " + std::to_string(move.targetY) + "}; ";
		out += "capture= ";
		out += (move.capture ? "true" : "false");
		out += "; ";

		auto piece_start = pos[move.startY][move.startX];
		auto piece_target = pos[move.targetY][move.targetX];
		auto capture = matt::ChessEngine::getCaptureValue(piece_start, piece_target);

		out += "capture_val= " + std::to_string((int)capture) + ": ";

		if (move.capture) out += matt::ChessEngine::getCaptureValueString(capture);
		else out += "-";

		out += "; simple_evaluation_val= " + std::to_string(matt::ChessEvaluation::evaluate(matt::ChessValidation::applyMove(pos, move), matt::PLAYER_WHITE, 0)) + "; ";
		out += "index= " + std::to_string(index);
		out += ")\n";
		index++;
	}

	std::cout << out;
}

auto main() -> int
{
	auto engine = matt::ChessEngine();
	// auto pos = matt::Position();


	std::array<char*, 6> test_fens_evaluation =
	{
		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
		"2k5/1p1pp3/8/5P2/6P1/1P6/P7/4K3 w - - 0 20",
		"4k3/1bb5/8/3n4/8/8/1B6/N1K4N w - - 0 13",
		"k7/r2p4/2p2b2/8/8/8/1PP3Q1/2B1K3 b - - 0 11",
		"1B6/5Rp1/p3k2p/8/3P4/1Q6/8/2K5 b - - 0 25",
		"8/1r6/K5k1/1p6/8/8/nR3q2/1b4P1 w - - 0 20"
	};

	for (auto fen : test_fens_evaluation)
	{
		std::cout << "------------------------------------------------------------\n";
		auto pos = matt::FENParser::fenToPosition(fen);
		float values[] = {
			matt::ChessEvaluation::evaluate(pos, matt::PLAYER_WHITE, 0),
			matt::ChessEvaluation::evaluate(pos, matt::PLAYER_WHITE, matt::EVAL_FT_STANDARD), 
			matt::ChessEvaluation::evaluate(pos, matt::PLAYER_WHITE, matt::EVAL_FT_ALL) 
		};

		matt::Position::printPosition(pos);
		std::string out = "";

		out += "\nPlayer\tMaterialOnly\tStandard\t+DynamicPawn\n";

		out += pos.getPlayer() == matt::PLAYER_WHITE ? "w\t" : "b\t";
		for (auto value : values)
		{
			out += std::to_string(value) + "\t";
		}
		out += "\n\n";

		std::cout << out;
	}

	// Testen
	//engine.searchMove(pos, matt::PLAYER_WHITE, 4, matt::FT_ALPHA_BETA | matt::FT_NESTED | matt::FT_SORT, false);
	//pos = matt::FENParser::fenToPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	//auto str = matt::FENParser::positionToFen(pos);
	//pos = matt::FENParser::fenToPosition(str);

	//float value = matt::ChessEvaluation::evaluate(pos, matt::PLAYER_WHITE, matt::EVAL_FT_STANDARD);

	//std::string test_capture_list = "8/1r6/K5k1/1p6/8/8/nR3q2/1b4P1 w - - 0 1";

	//pos = matt::FENParser::fenToPosition(test_capture_list);

	//auto moves = matt::ChessValidation::getValidMoves(pos, pos.getPlayer());

	//std::cout << "before sort: " << std::endl;
	//printMoves(moves, pos);

	//engine.sortMoves(&moves, pos);

	//std::cout << "after sort: " << std::endl;
	//printMoves(moves, pos);

	std::cout << "exit" << std::endl;
}
