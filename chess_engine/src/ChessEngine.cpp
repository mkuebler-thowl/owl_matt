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

	inline INT32 CALCULATE_MILLISECONDS(time_point start)
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

	Move ChessEngine::searchMove(short player, UINT16 depth, UCHAR parameterFlags, BOOL random)
	{
		// Die Position des generischen Zugs zur RepitionMap hinzufügen
		m_repitionMap.addPosition(m_position);

		m_mutex.lock();
		m_ready = false;
		m_stop = false;
		m_mutex.unlock();

		KILLER_LIST killer_list;
		MinMaxResult result;

		// MinMax oder MinMax+Sort
		//if (parameterFlags == FT_NULL || parameterFlags == FT_SORT)
		//{
		//	result = minMax(position, player, depth);
		//}

		// AlphaBeta oder AlphaBeta+Sort
		result = alphaBeta(m_position, player, depth, -INF, INF, 
			parameterFlags,	// Parameter-Flags
			parameterFlags & FT_SRT_KILLER ? &killer_list : nullptr // Killer-Liste
		);

		m_mutex.lock();
		m_ready = true;
		m_stop = false;
		m_mutex.unlock();

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

		m_position.applyMove(result.best);
		m_repitionMap.addPosition(m_position);

		return result.best;
	}

	BOOL ChessEngine::inDebugMode() const
	{
		return m_debugMode;
	}

	VOID ChessEngine::setDebugMode(BOOL debug)
	{
		m_debugMode = debug;
	}

	VOID ChessEngine::stop()
	{
		m_mutex.lock();
		m_stop = true;
		m_mutex.unlock();
	}

	BOOL ChessEngine::isReady() const
	{
		return m_ready;
	}

	VOID ChessEngine::setOption(const std::string& option, const std::string& value)
	{
		if (m_engineOptions.isSupported(option)) {
			m_engineOptions.setValue(option, value);
		}
	}

	const EngineOptions& ChessEngine::getOptions() const
	{
		return m_engineOptions;
	}

	const Position& ChessEngine::getPosition() const
	{
		return m_position;
	}

	Position& ChessEngine::getPosition()
	{
		return m_position;
	}

	//MinMaxResult ChessEngine::minMax(Position& position, short player, UINT16 depth)
	//{

	//	auto result = MinMaxResult{};
	//	result.values.resize(MIN_MAX_VALUE_SIZE);

	//	result.values[VALUE] = -player * INF;

	//	if (depth == 0)
	//	{
	//		// TODO: Change Engine Player
	//		result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
	//		
	//		return result;
	//	}

	//	auto moves = ChessValidation::getValidMoves(position, player);

	//	if (moves.empty())
	//	{
	//		// TODO: Change Engine Player
	//		result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);

	//		return result;
	//	}

	//	for (auto move : moves)
	//	{
	//		auto current_position = ChessValidation::applyMove(position, move);

	//		// Stellungen wegen 3x Stellungswiederholung überspringen
	//		if (m_repitionMap.isPositionAlreadyLocked(current_position)) continue;

	//		auto new_result = minMax(current_position, -player, depth - 1);

	//		if (new_result.values[VALUE] > result.values[VALUE] && player == PLAYER_WHITE || new_result.values[VALUE] < result.values[VALUE] && player == PLAYER_BLACK)
	//		{
	//			result.values[VALUE] = new_result.values[VALUE];
	//			result.best = move;
	//		}
	//		// if empty
	//		else if (new_result.values[VALUE] == result.values[VALUE] && result.best.startX == 0 && result.best.startY == 0 && result.best.targetX == 0 && result.best.targetY == 0)
	//		{
	//			result.best = move;
	//		}
	//	}
	//	return result;
	//}

	MinMaxResult ChessEngine::alphaBeta(Position& position, short player, UINT16 depth, FLOAT alpha, FLOAT beta, UCHAR parameterFlags, KILLER_LIST* killerList)
	{
		if (m_stop) return { INVALID_MOVE, {-INF}};

		auto result = MinMaxResult{};		
		auto is_player_white = player == PLAYER_WHITE;

		result.values.resize(MIN_MAX_VALUE_SIZE);
		result.values[VALUE] = -player * INF;

		if (depth == 0)
		{
			// TODO: Change Engine Player
			auto start = START_CLOCK();
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			m_v1 += CALCULATE_MICROSECONDS(start);
			m_v2++;
			return result;
		} 

		MOVE_LIST moves = ChessValidation::getValidMoves(position, player);

		if (moves.empty())
		{
			auto start = START_CLOCK();
			result.values[VALUE] = ChessEvaluation::evaluate(position, PLAYER_WHITE, EVAL_FT_STANDARD);
			m_v1 += CALCULATE_MICROSECONDS(start);
			m_v2++;
			return result;
		}

		sortMoves(&moves, position, depth, parameterFlags, killerList);

		for (auto move : moves)
		{
			position.applyMove(move);

			PAIR<FLOAT, FLOAT> alpha_beta = is_player_white ? 
				std::make_pair(result.values[VALUE], beta) : 
				std::make_pair(alpha, result.values[VALUE]);

			auto new_result = alphaBeta(position, -player, depth - 1, alpha_beta.first, alpha_beta.second, parameterFlags, killerList);

			position.undoLastMove();

			if (is_player_white && new_result.values[VALUE] > alpha)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;

				if (alpha >= beta) 
				{
					insertKiller(killerList, move, depth);
					break; 
				}
			}
			else if (!is_player_white && new_result.values[VALUE] < beta)
			{
				result.values[VALUE] = new_result.values[VALUE];
				result.best = move;

				if (beta <= alpha) 
				{
					insertKiller(killerList, move, depth);
					break;
				}
			}
		}
		
		return result;
	}

	VOID ChessEngine::sortMoves(MOVE_LIST* moves, Position& position, UINT16 depth, UCHAR parameterFlags, KILLER_LIST* killerList)
	{
		// Falls keine Sortierung aktiviert wurde: nichts tun
		if (parameterFlags < FT_BIT_SORT_BEGIN ) return;

		std::sort(moves->begin(), moves->end(), [&position, depth, parameterFlags, killerList](const Move& left, const Move& right)
		{
			// MVV_LVA: 
			if (parameterFlags & FT_SRT_MVV_LVA)
			{
				if (left.capture && !right.capture) return CHOOSE_LEFT;
				if (right.capture && !left.capture) return CHOOSE_RIGHT;

				if (left.capture && right.capture)
				{
					return getCaptureValue(position[left.startY][left.startX], position[left.targetY][left.targetX]) 
					> getCaptureValue(position[right.startY][right.startX], position[right.targetY][right.targetX]);
				}
			}

			// Killer-Heuristik:
			if (parameterFlags & FT_SRT_KILLER && killerList != nullptr)
			{
				auto left_prio = compareKiller(killerList, left, depth);
				if (left_prio == KILLER_PRIO_1) return CHOOSE_LEFT;

				auto right_prio = compareKiller(killerList, right, depth);
				if (right_prio == KILLER_PRIO_1) return CHOOSE_RIGHT;

				return left_prio > right_prio;
			}

			// Material-Heuristik
			if (parameterFlags & FT_SRT_MATERIAL)
			{
				FLOAT left_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, left), -position.getPlayer(), 0);
				FLOAT right_move_value = ChessEvaluation::evaluate(ChessValidation::applyMove(position, right), -position.getPlayer(), 0);

				return left_move_value > right_move_value;
			}
		});
	}

	Captures ChessEngine::getCaptureValue(CHAR attacker, CHAR victim)
	{
		attacker = std::tolower(attacker);
		victim = std::toupper(victim);

		switch (victim)
		{
		case WHITE_PAWN:
			switch (attacker)
			{
			case BLACK_PAWN: return Captures::pxP; break;
			case BLACK_KNIGHT: return Captures::nxP; break;
			case BLACK_BISHOP: return Captures::bxP; break;
			case BLACK_ROOK: return Captures::rxP; break;
			case BLACK_QUEEN: return Captures::qxP; break;
			case BLACK_KING: return Captures::kxP; break;
			}
			break;
		case WHITE_KNIGHT:
			switch (attacker)
			{
			case BLACK_PAWN: return Captures::pxN; break;
			case BLACK_KNIGHT: return Captures::nxN; break;
			case BLACK_BISHOP: return Captures::bxN; break;
			case BLACK_ROOK: return Captures::rxN; break;
			case BLACK_QUEEN: return Captures::qxN; break;
			case BLACK_KING: return Captures::kxN; break;
			}
			break;
		case WHITE_BISHOP:
			switch (attacker)
			{
			case BLACK_PAWN: return Captures::pxB; break;
			case BLACK_KNIGHT: return Captures::nxB; break;
			case BLACK_BISHOP: return Captures::bxB; break;
			case BLACK_ROOK: return Captures::rxB; break;
			case BLACK_QUEEN: return Captures::qxB; break;
			case BLACK_KING: return Captures::kxB; break;
			}
			break;
		case WHITE_ROOK:
			switch (attacker)
			{
			case BLACK_PAWN: return Captures::pxR; break;
			case BLACK_KNIGHT: return Captures::nxR; break;
			case BLACK_BISHOP: return Captures::bxR; break;
			case BLACK_ROOK: return Captures::rxR; break;
			case BLACK_QUEEN: return Captures::qxR; break;
			case BLACK_KING: return Captures::kxR; break;
			}
			break;
		case WHITE_QUEEN:
			switch (attacker)
			{
			case BLACK_PAWN: return Captures::pxQ; break;
			case BLACK_KNIGHT: return Captures::nxQ; break;
			case BLACK_BISHOP: return Captures::bxQ; break;
			case BLACK_ROOK: return Captures::rxQ; break;
			case BLACK_QUEEN: return Captures::qxQ; break;
			case BLACK_KING: return Captures::kxQ; break;
			}
			break;
		}

		return Captures::kxP;
	}

	std::string ChessEngine::getCaptureValueString(Captures capture)
	{
		return s_capture_map[static_cast<UINT64>(capture)];
	}
}