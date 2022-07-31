#include "ChessEngine.hpp"
#include "ChessValidation.hpp"
#include "ChessEvaluation.hpp"

#include <algorithm>

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
		if (sort) sortMoves(&moves, position);

		auto result = MinMaxResult{};

		if (depth == 0 || moves.empty())
		{
			result.value = evaluate(position, false);
			return result;
		}

		auto score = -INF * player;

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move);
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

	void ChessEngine::sortMoves(std::vector<Move>* moves, const Position& position)
	{
		std::sort(moves->begin(), moves->end(), [&position](const Move& left, const Move& right)
		{
			if (left.capture && !right.capture) return true;
			if (right.capture && !left.capture) return false;

			if (left.capture && right.capture)
			{
				float left_capture_value = 0.00f;
				float right_capture_value = 0.00f;

				left_capture_value -= ChessEvaluation::getMaterialValueByPiece(position[left.startY][left.startX]);
				left_capture_value += ChessEvaluation::getMaterialValueByPiece(position[left.targetY][left.targetX]);
				right_capture_value -= ChessEvaluation::getMaterialValueByPiece(position[right.startY][right.startX]);
				right_capture_value += ChessEvaluation::getMaterialValueByPiece(position[right.targetY][right.targetX]);

				return left_capture_value > right_capture_value;
			}

			float left_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, left), -position.getPlayer(), 0);
			float right_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, right), -position.getPlayer(), 0);

			return left_move_value > right_move_value;
		});
		
	}

	matt::ChessEngine::Captures ChessEngine::getCaptureValue(char attacker, char victim)
	{
		attacker = std::tolower(attacker);
		victim = std::toupper(victim);

		switch (victim)
		{
		case PAWN_WHITE:
			switch (attacker)
			{
			case PAWN_BLACK: return Captures::pxP; break;
			case KNIGHT_BLACK: return Captures::nxP; break;
			case BISHOP_BLACK: return Captures::bxP; break;
			case ROOK_BLACK: return Captures::rxP; break;
			case QUEEN_BLACK: return Captures::qxP; break;
			case KING_BLACK: return Captures::kxP; break;
			}
			break;
		case KNIGHT_WHITE:
			switch (attacker)
			{
			case PAWN_BLACK: return Captures::pxN; break;
			case KNIGHT_BLACK: return Captures::nxN; break;
			case BISHOP_BLACK: return Captures::bxN; break;
			case ROOK_BLACK: return Captures::rxN; break;
			case QUEEN_BLACK: return Captures::qxN; break;
			case KING_BLACK: return Captures::kxN; break;
			}
			break;
		case BISHOP_WHITE:
			switch (attacker)
			{
			case PAWN_BLACK: return Captures::pxB; break;
			case KNIGHT_BLACK: return Captures::nxB; break;
			case BISHOP_BLACK: return Captures::bxB; break;
			case ROOK_BLACK: return Captures::rxB; break;
			case QUEEN_BLACK: return Captures::qxB; break;
			case KING_BLACK: return Captures::kxB; break;
			}
			break;
		case ROOK_WHITE:
			switch (attacker)
			{
			case PAWN_BLACK: return Captures::pxR; break;
			case KNIGHT_BLACK: return Captures::nxR; break;
			case BISHOP_BLACK: return Captures::bxR; break;
			case ROOK_BLACK: return Captures::rxR; break;
			case QUEEN_BLACK: return Captures::qxR; break;
			case KING_BLACK: return Captures::kxR; break;
			}
			break;
		case QUEEN_WHITE:
			switch (attacker)
			{
			case PAWN_BLACK: return Captures::pxQ; break;
			case KNIGHT_BLACK: return Captures::nxQ; break;
			case BISHOP_BLACK: return Captures::bxQ; break;
			case ROOK_BLACK: return Captures::rxQ; break;
			case QUEEN_BLACK: return Captures::qxQ; break;
			case KING_BLACK: return Captures::kxQ; break;
			}
			break;
		}

		return Captures::kxP;
	}

	float ChessEngine::evaluate(const Position& position, bool simple)
	{
		return 0.0f;
	}
}