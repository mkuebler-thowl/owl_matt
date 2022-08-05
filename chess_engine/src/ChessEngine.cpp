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
		auto sort = parameterFlags & FT_SORT;

		MinMaxResult result;

		// MinMax oder MinMax+Sort
		if (parameterFlags == FT_NULL || parameterFlags == FT_SORT)
		{
			result = minMax(position, player, depth, sort);
		}
		// AlphaBeta oder AlphaBeta+Sort
		else if (parameterFlags == FT_ALPHA_BETA || parameterFlags == (FT_ALPHA_BETA | FT_SORT))
		{ 
			result = alphaBeta(position, player, depth, -INF, INF, sort);
		}

		//std::cout << (parameter_flags == (FT_ALPHA_BETA | FT_NESTED) || parameter_flags == (FT_ALPHA_BETA | FT_NESTED | FT_SORT)) << std::endl;
		//std::cout << (parameter_flags == FT_NESTED || parameter_flags == (FT_NESTED | FT_SORT)) << std::endl;

		return result.best;
	}

	MinMaxResult ChessEngine::minMax(const Position& position, short player, unsigned short depth, bool sort)
	{
		auto moves = ChessValidation::getValidMoves(position, player, sort);

		if (sort) sortMoves(&moves, position);

		auto result = MinMaxResult{};
		result.values.resize(MIN_MAX_VALUE_SIZE);

		if (depth == 0 || moves.empty())
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			
			return result;
		}
		else
		{
			m_totalMoveCount += moves.size();
		}

		for (auto move : moves)
		{
			m_iterationCount++;
			auto current_position = ChessValidation::applyMove(position, move);
			auto new_result = minMax(current_position, -player, depth - 1, sort);

			if (new_result.values[VALUE] > result.values[VALUE] && player == PLAYER_WHITE || new_result.values[VALUE] < result.values[VALUE] && player == PLAYER_BLACK)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;
			}
		}

		return result;
	}

	MinMaxResult ChessEngine::alphaBeta(const Position& position, short player, unsigned short depth, float alpha, float beta, bool sort)
	{
		auto moves = ChessValidation::getValidMoves(position, player, sort);

		if (sort) sortMoves(&moves, position);

		// DEBUG:
		std::cout << "\n";

		for (auto move : moves)
		{
			std::cout << "[FOUND]: ";
			Move::printMove(move);
		}
		std::cout << "\n";

		auto is_player_white = player == PLAYER_WHITE;

		auto result = MinMaxResult{};
		result.values.resize(ALPHA_BETA_SIZE);

		result.values[VALUE] = -player * INF;
		result.values[ALPHA] = alpha;
		result.values[BETA] = beta;

		if (depth == 0 || moves.empty())
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);

			if (is_player_white) result.values[ALPHA] = result.values[VALUE];
			else result.values[BETA] = result.values[VALUE];

			return result;
		} 
		else
		{
			m_totalMoveCount += moves.size();
		}

		for (auto move : moves)
		{
			m_iterationCount++;
			auto current_position = ChessValidation::applyMove(position, move);

			auto new_result = alphaBeta(current_position, -player, depth - 1, result.values[ALPHA], result.values[BETA], sort);

			if (is_player_white && new_result.values[VALUE] > result.values[ALPHA])
			{
				result.values[ALPHA] = new_result.values[VALUE];
				result.values[VALUE] = result.values[ALPHA];
				result.best = move;

				std::cout << "[NEW BEST white]: ";
				Move::printMove(move);

				if (alpha > beta) { std::cout << "[-PRUNED a>b]: "; Move::printMove(move); m_pruneCount++; break; }
			}
			else if (!is_player_white && new_result.values[VALUE] < result.values[BETA])
			{
				result.values[BETA] = new_result.values[VALUE];
				result.values[VALUE] = result.values[BETA];
				result.best = move;

				std::cout << "[NEW BEST black]: ";
				Move::printMove(move);

				if (beta < alpha) { std::cout << "[-PRUNED b<a]: "; Move::printMove(move); m_pruneCount++; break; }
			}
		}
	
		std::cout << "[FINAL BEST result]: ";
		Move::printMove(result.best);

		return result;
	}

	void ChessEngine::sortMoves(std::vector<Move>* moves, const Position& position)
	{
		std::sort(moves->begin(), moves->end(), [&position](const Move& left, const Move& right)
		{
			if (left.capture && !right.capture) return true;
			if (right.capture && !left.capture) return false;

			if (left.capture && right.capture)
			{
				return getCaptureValue(position[left.startY][left.startX], position[left.targetY][left.targetX]) > getCaptureValue(position[right.startY][right.startX], position[right.targetY][right.targetX]);
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

	std::string ChessEngine::getCaptureValueString(Captures capture)
	{
		return s_capture_map[static_cast<size_t>(capture)];
	}
}