#include "ChessEngine.hpp"
#include "ChessValidation.hpp"
#include "ChessEvaluation.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include "ChessUtility.hpp"

namespace owl
{

	//static MinMaxResult m_result;

	ChessEngine::ChessEngine()
		: m_player(0)
	{
		ChessValidation::init();
	}
	ChessEngine::~ChessEngine()
	{
	}

	PAIR<Move, EVALUATION_VALUE> ChessEngine::searchMove(INT32 player, INT32 depth, UCHAR parameterFlags, BOOL random)
	{
		m_mutex.lock();

		m_ready = false;
		m_stop = false;
		m_startedDepth = depth;
		m_repitionMap.addPosition(m_position);
		if(m_player == 0) m_player = player;
		KILLER_LIST killer_list;

		m_mutex.unlock();

		auto time_start = std::chrono::steady_clock::now();
		// Führe MinMax aus:
		if (parameterFlags == FT_NULL) minMax(m_position, m_player, depth);
		// Führe Alpha-Beta aus
		else alphaBeta(m_position, m_player, depth, -INF, INF, 
			parameterFlags,	// Parameter-Flags
			parameterFlags & FT_SRT_KILLER ? &killer_list : nullptr // Killer-Liste
		);
		auto time_end = std::chrono::steady_clock::now();

		std::cout << "info searchtime " << (std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count()) << " ms" << std::endl;

		m_mutex.lock();

		m_ready = true;
		m_stop = false;
		auto result = m_result.getResult();
		m_position.applyMove(result.first);
		m_repitionMap.addPosition(m_position);

		m_mutex.unlock();

		return result;
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

	VOID ChessEngine::setPosition(const Position& position)
	{
		if (m_player == 0) m_player = position.getPlayer();
		m_position = position;
	}

	const Position& ChessEngine::getPosition() const
	{
		return m_position;
	}

	INT32 ChessEngine::getPlayer() const
	{
		return m_player;
	}

	Position& ChessEngine::getPosition()
	{
		return m_position;
	}

	EVALUATION_VALUE ChessEngine::minMax(Position& position, INT32 player, INT32 depth)
	{
		if (m_stop) return -INF;

		auto value = -player * INF;

		if (depth == 0)
		{
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD);
		}

		auto moves = ChessValidation::getValidMoves(position, player);

		if (moves.empty())
		{
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD);
		}
		else if (moves.size() == 1 && depth == m_startedDepth)
		{
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD);
		}

		for (auto &move : moves)
		{
			position.applyMove(move);

			auto new_value = minMax(position, -player, depth - 1);

			position.undoLastMove();

			if (new_value > value && player == m_player 
				|| new_value < value && player != m_player)
			{
				value = new_value;
				if (depth == m_startedDepth) {
					m_result.insert(move, value);
				}
			}
			else if (new_value == value && m_result.empty())
			{
				if (depth == m_startedDepth) {
					m_result.insert(move, value);
				}
			}
		}

		return value;
	}

	EVALUATION_VALUE ChessEngine::alphaBeta(Position& position, INT32 player, INT32 depth, FLOAT alpha, FLOAT beta, UCHAR parameterFlags, KILLER_LIST* killerList)
	{		
		if (m_stop)
		{
			return -INF;
		}

		// Blattknoten erreicht?
		if (depth == 0)
		{
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD, true);
		} 

		// Liste generieren + Checkmate überprüfen bei 0 Zügen
		MOVE_LIST moves = ChessValidation::getValidMoves(position, player); 

		// Endstellung erreicht? 
		if (moves.empty())
		{
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD);
		}

		// Züge gegebenfalls sortieren
		sortMoves(&moves, position, depth, parameterFlags, killerList);
		
		EVALUATION_VALUE value = player == PLAYER_WHITE ? alpha : beta;

		for (auto move : moves)
		{
			position.applyMove(move);

			auto new_alpha = player == PLAYER_WHITE ? value : alpha;
			auto new_beta = player == PLAYER_WHITE ? beta : value;

			EVALUATION_VALUE new_value = alphaBeta(position, -player, depth - 1, new_alpha, new_beta, parameterFlags, killerList);

			position.undoLastMove();

			if (player == PLAYER_WHITE && new_value > value)
			{
				value = new_value;

				if (depth == m_startedDepth) {
					m_result.insert(move, value);
				}

				if (value >= beta)
				{
					insertKiller(killerList, move, depth);
					break; 
				}
			}
			else if (player == PLAYER_BLACK && new_value < value)
			{
				value = new_value;

				if (depth == m_startedDepth) {
					m_result.insert(move, value);
				}

				if (value <= alpha)
				{
					insertKiller(killerList, move, depth);
					break;
				}
			}
		}
		
		return value;
	}

	VOID ChessEngine::sortMoves(MOVE_LIST* moves, Position& position, INT32 depth, UCHAR parameterFlags, KILLER_LIST* killerList)
	{
		// Falls keine Sortierung aktiviert wurde: nichts tun
		if (parameterFlags < FT_BIT_SORT_BEGIN ) return;

		auto& enginePlayer = m_player;

		std::sort(moves->begin(), moves->end(), [&position, depth, parameterFlags, killerList, &enginePlayer](const Move& left, const Move& right)
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
				position.applyMove(left);
				FLOAT left_move_value = ChessEvaluation::evaluate(position, enginePlayer, 0);
				position.undoLastMove();
				position.applyMove(right);
				FLOAT right_move_value = ChessEvaluation::evaluate(position, enginePlayer, 0);
				position.undoLastMove();

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