#include "ChessEngine.hpp"
#include "ChessValidation.hpp"
#include "ChessEvaluation.hpp"

#include <algorithm>
#include <chrono>





namespace owl
{
	using clock = std::chrono::steady_clock;
	using time_point = std::chrono::steady_clock::time_point;
	using milliseconds = std::chrono::milliseconds;
	using microseconds = std::chrono::microseconds;

	inline time_point START_CLOCK()
	{
		return clock::now();
	}

	inline int CALCULATE_MILLISECONDS(time_point start)
	{
		return std::chrono::duration_cast<milliseconds>(clock::now() - start).count();
	}

	inline auto CALCULATE_MICROSECONDS(time_point start)
	{
		return std::chrono::duration_cast<microseconds>(clock::now() - start).count();
	}

	ChessEngine::ChessEngine()
	{

	}
	ChessEngine::~ChessEngine()
	{
	}

	Move ChessEngine::searchMove(const Position& position, short player, unsigned short depth, unsigned char parameterFlags, bool random)
	{
		// Die Position des generischen Zugs zur RepitionMap hinzufügen
		m_repitionMap.addPosition(position);

		bool sort = parameterFlags & FT_SORT;
		bool killer = parameterFlags & FT_KILLER;

		MinMaxResult result;

		// MinMax oder MinMax+Sort
		if (parameterFlags == FT_NULL || parameterFlags == FT_SORT)
		{
			result = minMax(position, player, depth);
		}
		// AlphaBeta oder AlphaBeta+Sort
		else if (parameterFlags & FT_ALPHA_BETA)
		{ 
			result = alphaBeta(position, player, depth, -INF, INF, sort, killer);
		}

		//// Nested oder Nested+Sort
		//else if (parameterFlags == FT_NESTED || parameterFlags == (FT_NESTED | FT_SORT))
		//{
		//	result = nested(position, player, depth, sort);
		//}
		//// Nested+AlphaBeta oder Nested+AlphaBeta+Sort
		//else if (parameterFlags == (FT_NESTED | FT_ALPHA_BETA) || parameterFlags == (FT_NESTED | FT_ALPHA_BETA | FT_SORT))
		//{
		//	result = nestedAlphaBeta(position, player, depth, -INF, INF, sort);
		//}

		//std::cout << (parameter_flags == (FT_ALPHA_BETA | FT_NESTED) || parameter_flags == (FT_ALPHA_BETA | FT_NESTED | FT_SORT)) << std::endl;
		//std::cout << (parameter_flags == FT_NESTED || parameter_flags == (FT_NESTED | FT_SORT)) << std::endl;

		m_repitionMap.addPosition(ChessValidation::applyMove(position, result.best));

		return result.best;
	}

	MinMaxResult ChessEngine::minMax(const Position& position, short player, unsigned short depth)
	{


		auto result = MinMaxResult{};
		result.values.resize(MIN_MAX_VALUE_SIZE);

		result.values[VALUE] = -player * INF;

		if (depth == 0)
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			
			return result;
		}

		auto moves = ChessValidation::getValidMoves(position, player);

		if (moves.empty())
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);

			return result;
		}

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move);

			// Stellungen wegen 3x Stellungswiederholung überspringen
			if (m_repitionMap.isPositionAlreadyLocked(current_position)) continue;

			auto new_result = minMax(current_position, -player, depth - 1);

			if (new_result.values[VALUE] > result.values[VALUE] && player == PLAYER_WHITE || new_result.values[VALUE] < result.values[VALUE] && player == PLAYER_BLACK)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;
			}
			// if empty
			else if (new_result.values[VALUE] == result.values[VALUE] && result.best.startX == 0 && result.best.startY == 0 && result.best.targetX == 0 && result.best.targetY == 0)
			{
				result.best = move;
			}
		}
		return result;
	}

	MinMaxResult ChessEngine::alphaBeta(const Position& position, short player, unsigned short depth, float alpha, float beta, bool sort, bool killer)
	{
		auto result = MinMaxResult{};		
		auto is_player_white = player == PLAYER_WHITE;

		result.values.resize(MIN_MAX_VALUE_SIZE);
		result.values[VALUE] = -player * INF;

		if (depth == 0)
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			return result;
		} 

		std::vector<Move> moves = ChessValidation::getValidMoves(position, player);

		if (moves.empty())
		{
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			return result;
		}

		auto start = START_CLOCK();
		if (sort) sortMoves(&moves, position, depth, &result, killer);
		m_v1 += CALCULATE_MICROSECONDS(start);
		m_v2++;

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move);
			
			//// Stellungen wegen 3x Stellungswiederholung überspringen
			//if (m_repitionMap.isPositionAlreadyLocked(current_position)) continue;

			std::pair<float, float> alpha_beta = is_player_white ? 
				std::make_pair(result.values[VALUE], beta) : 
				std::make_pair(alpha, result.values[VALUE]);

			auto new_result = alphaBeta(current_position, -player, depth - 1, alpha_beta.first, alpha_beta.second, sort, killer);
			
			if (killer)
			{
				for (int d = 0; d <= depth; d++)
				{
					result.killers[d].merge(new_result.killers[d]);
				}
			}

			if (is_player_white && new_result.values[VALUE] > alpha)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;


				if (alpha >= beta) 
				{ 
					if (killer) result.killers[depth].insert(move);
					//m_count++;  
					break; 
				}
			}
			else if (!is_player_white && new_result.values[VALUE] < beta)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;

				if (beta <= alpha) 
				{
					if (killer) result.killers[depth].insert(move);
					//m_count++;
					break; 
				}
			}
		}

		return result;
	}

	MinMaxResult ChessEngine::nested(const Position& position, short player, unsigned short depth, bool sort)
	{
		auto moves = ChessValidation::getValidMoves(position, player);
		//if (sort) sortMoves(&moves, position, depth, result, sort);
		
		MinMaxResult result;
		result.values.resize(NESTED_SIZE);

		if (position.getGameState() != GameState::Active)
		{
			auto value = ChessEvaluation::evaluate(position, PLAYER_WHITE, 0);

			result.values[VALUE] = value;
			result.values[LOWER] = value;
			result.values[UPPER] = value;

			return result;
		}

		auto is_player_white = player == PLAYER_WHITE;

		if (depth == 0 || moves.empty())
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			result.values[LOWER] = -INF;
			result.values[UPPER] = INF;

			//if (is_player_white) result.values[ALPHA] = result.values[VALUE];
			//else result.values[BETA] = result.values[VALUE];

			return result;
		}

		result.values[VALUE] = -player * INF;
		result.values[LOWER] = -INF;
		result.values[UPPER] = -INF;

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move);
			auto new_result = nested(current_position, -player, depth - 1, sort);

			if (new_result.values[VALUE] > result.values[VALUE] && player == PLAYER_WHITE || new_result.values[VALUE] < result.values[VALUE] && player == PLAYER_BLACK)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.values[LOWER] = std::max(result.values[LOWER], -new_result.values[UPPER]);
				result.values[UPPER] = std::max(result.values[UPPER], -new_result.values[LOWER]);
				result.best = move;
			}
			else if (new_result.values[VALUE] == result.values[VALUE] && result.best.startX == 0 && result.best.startY == 0 && result.best.targetX == 0 && result.best.targetY == 0)
			{
				result.best = move;
			}
		}

		return result;
	}

	MinMaxResult ChessEngine::nestedAlphaBeta(const Position& position, short player, unsigned short depth, float alpha, float beta, bool sort)
	{
		MinMaxResult result;
		result.values.resize(NESTED_SIZE);

		if (position.getGameState() != GameState::Active)
		{
			auto value = ChessEvaluation::evaluate(position, PLAYER_WHITE, 0);

			result.values[VALUE] = value;
			result.values[LOWER] = value;
			result.values[UPPER] = value;

			return result;
		}

		auto moves = ChessValidation::getValidMoves(position, player);
		//if (sort) sortMoves(&moves, position);

		auto is_player_white = player == PLAYER_WHITE;

		if (depth == 0 || moves.empty())
		{
			// TODO: Change Engine Player
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			result.values[LOWER] = -INF;
			result.values[UPPER] = INF;

			if (is_player_white) result.values[ALPHA] = result.values[VALUE];
			else result.values[BETA] = result.values[VALUE];

			return result;
		}

		result.values[VALUE] = -player * INF;
		result.values[LOWER] = -INF;
		result.values[UPPER] = -INF;
		result.values[ALPHA] = alpha;
		result.values[BETA] = beta;

		for (auto move : moves)
		{
			auto current_position = ChessValidation::applyMove(position, move);
			auto new_result = nestedAlphaBeta(current_position, -player, depth - 1, result.values[ALPHA], result.values[BETA], sort);

			if (is_player_white && new_result.values[VALUE] > result.values[ALPHA])
			{
				result.values[ALPHA] = new_result.values[VALUE];
				result.values[VALUE] = result.values[ALPHA];
				result.values[LOWER] = std::max(result.values[LOWER], -new_result.values[UPPER]);
				result.values[UPPER] = std::max(result.values[UPPER], -new_result.values[LOWER]);
				result.best = move;

				if (alpha > beta) {  break; }
			}
			else if (!is_player_white && new_result.values[VALUE] < result.values[BETA])
			{
				result.values[BETA] = new_result.values[VALUE];
				result.values[VALUE] = result.values[BETA];
				result.values[LOWER] = std::max(result.values[LOWER], -new_result.values[UPPER]);
				result.values[UPPER] = std::max(result.values[UPPER], -new_result.values[LOWER]);
				result.best = move;

				if (beta < alpha) {  break; }
			}
		}

		return result;
	}

	void ChessEngine::sortMoves(std::vector<Move>* moves, const Position& position, unsigned short depth, MinMaxResult* pResult, bool killer)
	{

		std::sort(moves->begin(), moves->end(), [&position, depth, pResult, killer](const Move& left, const Move& right)
		{
			if (left.capture && !right.capture) return true;
			if (right.capture && !left.capture) return false;

			if (left.capture && right.capture)
			{
				return getCaptureValue(position[left.startY][left.startX], position[left.targetY][left.targetX]) > getCaptureValue(position[right.startY][right.startX], position[right.targetY][right.targetX]);
			}

			// Heuristics
			if (killer)
			{
				if (pResult->killers.size() > 0)
					std::cout << "huh" << std::endl;

				auto killer_left = killerHeuristics(left, depth, pResult);
				auto killer_right = killerHeuristics(right, depth, pResult);
				if (killer_left && !killer_right) return true;
				else if (!killer_left && killer_right) return false;
			}

			float left_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, left), -position.getPlayer(), 0);
			float right_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, right), -position.getPlayer(), 0);

			return left_move_value > right_move_value;
		});
	}

	bool ChessEngine::killerHeuristics(const Move& move, unsigned short depth, MinMaxResult* pResult)
	{
		if (pResult->killers.find(depth) != pResult->killers.end())
		{
			for (Move killer : pResult->killers[depth])
			{
				if (killer == move) { std::cout << "killer found\n";  return true; }
			}
		}
		return false;
	}

	owl::ChessEngine::Captures ChessEngine::getCaptureValue(char attacker, char victim)
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