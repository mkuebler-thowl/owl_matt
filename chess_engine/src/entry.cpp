#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

#include "defines.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "ChessEngine.hpp"
#include "ChessEvaluation.hpp"
#include "FENParser.hpp"

// Test: Print move list
auto printMoves(std::vector<owl::Move> moves, owl::Position& pos) -> owl::VOID
{
	std::string out = "";

	owl::INT32 index = 0;

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
		auto capture = owl::ChessEngine::getCaptureValue(piece_start, piece_target);

		out += "capture_val= " + std::to_string((owl::INT32)capture) + ": ";

		if (move.capture) out += owl::ChessEngine::getCaptureValueString(capture);
		else out += "-";

		out += "; simple_evaluation_val= " + std::to_string(owl::ChessEvaluation::evaluate(owl::ChessValidation::applyMove(pos, move), owl::PLAYER_WHITE, 0)) + "; ";
		out += "index= " + std::to_string(index);
		out += ")\n";
		index++;
	}

	std::cout << out;
}

auto testEvaluationFunction()
{

	std::vector<owl::CHAR*> test_fens_evaluation =
	{
		"r2qk2r/ppp2ppp/2nbbn2/3pp3/8/P2PP3/NPP2PPP/R1BQKBNR b KQkq - 4 7", // 1
		"2k5/4pp2/4p1p1/1Pp3P1/P4P2/4P3/8/4K3 w - - 0 1",					// 2
		"2r1r2k/5p1p/q4p2/2p5/p1P2R2/P5QP/5PP1/3R2K1 w - - 0 1",			// 3

		"2k5/ppn2nb1/b1p1p1pp/3p1p2/3P1P2/B1P1P1PP/PPN2NB1/2K5 w - - 0 1",	// 4vs4
		"2k5/4b3/5r2/r3q3/R3Q3/5R2/4B3/2K5 w - - 0 1",						// 5vs5 -
		"8/3k4/3r4/r1q5/R1Q5/3R4/3K4/8 w - - 0 1",							// 6vs6
		
		"4k3/pppppppp/8/8/3P1P2/B1P1P1PP/PPN2NB1/2K5 w - - 0 1",			// 4vs Bauernlinie+k
		"2k5/4b3/5r2/r3q3/3P1P2/B1P1P1PP/PPN2NB1/2K5 w - - 0 1",			// 4vs5 - 
		"8/3k4/3r4/r1q5/3P1P2/B1P1P1PP/PPN2NB1/2K5 w - - 0 1",				// 4vs6

		"4k3/pppppppp/8/8/R3Q3/5R2/4B3/2K5 w - - 0 1",						// 5vs Bauernlinie+k - 
		"8/3k4/3r4/r1q5/R3Q3/5R2/4B3/2K5 w - - 0 1",						// 5vs6 -

		"4k3/pppppppp/8/8/R1Q5/3R4/3K4/8 w - - 0 1",						// 6vs Bauernlinie+k

		//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
		//"2k5/1p1pp3/8/5P2/6P1/1P6/P7/4K3 w - - 0 20",
		//"4k3/1bb5/8/3n4/8/8/1B6/N1K4N w - - 0 20",
		//"k7/r2p4/2p2b2/8/8/8/1PP3Q1/2B1K3 b - - 0 24",
		//"1B6/5Rp1/p3k2p/8/3P4/1Q6/8/2K5 b - - 0 25",
	};

	for (auto fen : test_fens_evaluation)
	{
		std::cout << "------------------------------------------------------------------------\n";
		std::cout << "\nFEN: " << fen << " pos:" << std::endl;
		std::cout << "------------------------------------------------------------------------\n";
		auto pos = owl::FENParser::fenToPosition(fen);
		std::string values[] = {
			"\nMaterial: ",
			std::to_string(owl::ChessEvaluation::evaluate(pos, owl::PLAYER_WHITE, 0)),
			"Value: ",
			std::to_string(owl::ChessEvaluation::evaluate(pos, owl::PLAYER_WHITE, owl::EVAL_FT_STANDARD)),
			"DynamicPawnValue: ",
			std::to_string(owl::ChessEvaluation::evaluate(pos, owl::PLAYER_WHITE, owl::EVAL_FT_ALL)),
			"GamePhase{0=opening,1=mid,2=end}:",
			std::to_string((owl::INT32)pos.getGamePhase())
		};

		owl::Position::printPosition(pos);
		std::cout << "------------------------------------------------------------------------\n";
		std::string out = "";

		owl::INT32 index = 0;

		for (auto value : values)
		{
			if(index % 2 == 0) out += value + " ";
			else out += value + "\n";
			index++;
		}
		out += "\n\n";

		std::cout << out;
	}
}

auto main() -> owl::INT32
{
	auto engine = owl::ChessEngine();
	
	auto pos = owl::FENParser::fenToPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	auto move = engine.searchMove(pos, pos.getPlayer(),6, owl::FT_ALPHA_BETA | owl::FT_SORT | owl::FT_KILLER);

	std::cout << "exit" << std::endl;
}
