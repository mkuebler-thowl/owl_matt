#include "ChessEngine.hpp"
#include "ChessValidation.hpp"

#include <iostream>

namespace matt
{
	ChessEngine::ChessEngine()
	{

	}
	ChessEngine::~ChessEngine()
	{
	}

	Move ChessEngine::searchMove(const Position& position, short player, unsigned short depth, unsigned char parameterFlags, bool random)
	{
		auto sort = parameterFlags | FT_SORT;

		// MinMax oder MinMax+Sort
		if (parameterFlags == FT_NULL || parameterFlags == FT_SORT)
		{
			auto value = minMax(position, player, depth, sort);
		}
		// AlphaBeta oder AlphaBeta+Sort
		else if (parameterFlags == FT_ALPHA_BETA || parameterFlags == (FT_ALPHA_BETA | FT_SORT))
		{ 
			auto value = alphaBeta(position, player, depth, -INF, INF, sort);
			
		}

		//std::cout << (parameter_flags == (FT_ALPHA_BETA | FT_NESTED) || parameter_flags == (FT_ALPHA_BETA | FT_NESTED | FT_SORT)) << std::endl;
		//std::cout << (parameter_flags == FT_NESTED || parameter_flags == (FT_NESTED | FT_SORT)) << std::endl;

		return Move();
	}

	MinMaxResult ChessEngine::minMax(const Position& position, short player, unsigned short depth, bool sort)
	{
		auto moves = ChessValidation::getValidMoves(position, sort);
		auto result = MinMaxResult{};

		if (depth == 0 || moves.empty())
		{
			result.value = evaluate(position, false);
			return result;
		}

		auto score = -INF * player;

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move, player);
			result = minMax(current_position, -player, depth - 1, sort);

			if (result.value > score && player == PLAYER_WHITE || result.value < score && player == PLAYER_BLACK)
			{
				score == result.value;
				result.best = move;
			}
		}

		return result;
	}

	float ChessEngine::alphaBeta(const Position& position, short player, unsigned short depth, float alpha, float beta, bool sort)
	{
		return 0.0f;
	}
	float ChessEngine::evaluate(const Position& position, bool simple)
	{
		return 0.0f;
	}
}