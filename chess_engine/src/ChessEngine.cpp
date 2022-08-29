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

	PAIR<Move, EVALUATION_VALUE> ChessEngine::searchMove(INT32 player, INT32 depth, UCHAR parameterFlags)
	{
		m_mutex.lock();

		m_ready = false;
		m_stop = false;
		m_startedDepth = depth;
		m_repitionMap.addPosition(m_position);
		if(m_player == 0) m_player = player;

		// Testwerte für Effizienz:
		m_nodes = 0;
		m_prunes = 0;

		m_mutex.unlock();

		auto time_start = std::chrono::steady_clock::now();

		// Wenn Tiefe = 0: einfach die Position direkt evaluieren
		if (depth == 0)
		{
			m_result.insert(INVALID_MOVE, ChessEvaluation::evaluate(m_position, player, EVAL_FT_STANDARD, true), player == m_player);
		}
		else minMax(m_position, m_player, depth, -INF, INF, parameterFlags);
		auto time_end = std::chrono::steady_clock::now();
		m_searchTime = (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count());

#if OWL_LOG_NO_INFO==false
		std::cout << "info searchtime " << m_searchTime << " ms" << std::endl;
#endif
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

	INT32 ChessEngine::getPrunesCount() const
	{
		return m_prunes;
	}

	INT32 ChessEngine::getNodesCount() const
	{
		return m_nodes;
	}

	INT64 ChessEngine::getSearchTime() const
	{
		return m_searchTime;
	}

	Position& ChessEngine::getPosition()
	{
		return m_position;
	}

	EVALUATION_VALUE ChessEngine::minMax(Position& position, INT32 player, INT32 depth, FLOAT alpha, FLOAT beta, UCHAR parameterFlags)
	{		
		if (m_stop)
		{
			return -INF;
		}

		m_nodes++;

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
			return ChessEvaluation::evaluate(position, m_player, EVAL_FT_STANDARD, false);
		}

		// Züge gegebenfalls sortieren
		sortMoves(&moves, position, depth, parameterFlags);
		
		EVALUATION_VALUE value = player == m_player ? alpha : beta;

		for (auto& move : moves)
		{
			position.applyMove(move);

			FLOAT new_alpha = player == m_player ? static_cast<FLOAT>(value) : alpha;
			FLOAT new_beta = player == m_player ? beta : static_cast<FLOAT>(value);

			EVALUATION_VALUE new_value = minMax(position, -player, depth - 1, new_alpha, new_beta, parameterFlags);

			position.undoLastMove();

			if (player == m_player && new_value > static_cast<FLOAT>(value)-RANDOM_THRESHOLD)
			{
            #if OWL_LOG_MOVE_STACK==true
                if (depth == 1)
                {
                    std::cout << "info update old_value: {" << value << "} value: {" << new_value << "}\n";
                    position.applyMove(move);
                    std::stack<Position::MoveData> _mvs = position.getMoveDataStack();
                    std::vector<Position::MoveData> _vmvs;
                    
                    while(_mvs.size() > 1)
                    {
                        _vmvs.insert(_vmvs.begin(), _mvs.top());
                        _mvs.pop();
                    }
                    
                    std::cout << "info update fen " << ChessUtility::positionToFen(position) << "\n";
                    
                    std::cout << "info update moves ";
                    for(auto& item : _vmvs)
                    {
                        std::cout << ChessUtility::moveToString(item.move) << " ";
                    }
                    std::cout << "\n\n";
                    position.undoLastMove();
                }
            #endif

				if (new_value > value) value = new_value;
                
				if (depth == m_startedDepth) {
					m_result.insert(move, new_value, true);

					if (parameterFlags & FT_ALPHA_BETA && static_cast<FLOAT>(value) - RANDOM_THRESHOLD >= beta)
					{
						insertKiller(move, depth);
						m_prunes++;
						break;
					}
				}
				else if (parameterFlags & FT_ALPHA_BETA && static_cast<FLOAT>(value) >= beta)
				{
					insertKiller(move, depth);
					m_prunes++;
					break;
				}
			}
			else if (player != m_player && new_value < static_cast<FLOAT>(value)+RANDOM_THRESHOLD)
			{
            #if OWL_LOG_MOVE_STACK==true
                if (depth == 1)
                {
                    std::cout << "info update old_value: {" << value << "} value: {" << new_value << "}\n";
                    position.applyMove(move);
                    std::stack<Position::MoveData> _mvs = position.getMoveDataStack();
                    std::vector<Position::MoveData> _vmvs;
                    
                    while(_mvs.size() > 1)
                    {
                        _vmvs.insert(_vmvs.begin(), _mvs.top());
                        _mvs.pop();
                    }
                    
                    std::cout << "info update fen " << ChessUtility::positionToFen(position) << "\n";
                    
                    std::cout << "info update moves ";
                    for(auto& item : _vmvs)
                    {
                        std::cout << ChessUtility::moveToString(item.move) << " ";
                    }
                    std::cout << "\n\n";
                    position.undoLastMove();
                    
                }
            #endif
				if (new_value < value) value = new_value;
                
				if (depth == m_startedDepth) {
					m_result.insert(move, new_value, false);

					if (parameterFlags & FT_ALPHA_BETA && static_cast<FLOAT>(value) + RANDOM_THRESHOLD <= alpha)
					{
						insertKiller(move, depth);
						m_prunes++;
						break;
					}
				}
				else if (parameterFlags & FT_ALPHA_BETA && static_cast<FLOAT>(value) <= alpha)
				{
					insertKiller(move, depth);
					m_prunes++;
					break;
				}
			}
		}

		return value;
	}

	VOID ChessEngine::sortMoves(MOVE_LIST* moves, Position& position, INT32 depth, UCHAR parameterFlags)
	{
		// Falls keine Sortierung aktiviert wurde: nichts tun
		if (parameterFlags < FT_BIT_SORT_BEGIN ) return;

		auto& enginePlayer = m_player;

		std::sort(moves->begin(), moves->end(), [this, &position, depth, parameterFlags, &enginePlayer](const Move& left, const Move& right)
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
			if (parameterFlags & FT_SRT_KILLER)
			{
				// Linke Seite mit Killer vergleichen
				auto left_prio = compareKiller(left, depth);
				if (left_prio == KILLER_PRIO_1) return CHOOSE_LEFT;

				// Rechte Seite mit Killer vergleichen
				auto right_prio = compareKiller(right, depth);
				if (right_prio == KILLER_PRIO_1) return CHOOSE_RIGHT;

				// Falls einer der beiden Seiten ein Killerzug sind vergleichen
				if (left_prio > KILLER_NO_PRIO || right_prio > KILLER_NO_PRIO)
					return left_prio > right_prio;
				// Ansonsten fortfahren
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

			// Unterdrückung der Warnung: Wenn alles klappt, wird 'return true' gar nicht erst erreicht
			return false; 
		});
	}

	VOID ChessEngine::insertKiller(const Move move, const INT32 ply)
	{
		// Ignoriere Captures, invalide Züge und bereits vorhandene Züge in der Killer-Liste
		if (move.capture) return;
		if (move.isMoveInvalid()) return;
		if (move == m_killerList[FIRST_KILLER_INDEX][ply] || move == m_killerList[LAST_KILLER_INDEX][ply]) return;
		// Ansonten:

		// Verschiebe Zug: 1. Feld => 2. Feld
		m_killerList[LAST_KILLER_INDEX][ply] = m_killerList[FIRST_KILLER_INDEX][ply];

		// Platziere neuen Zug: 1. Feld
		m_killerList[FIRST_KILLER_INDEX][ply] = move;
	}

	INT32 ChessEngine::compareKiller(const Move move, const INT32 ply)
	{
		// Wurzelknoten?
		if (ply >= MAX_DEPTH) return KILLER_NO_PRIO;

		// Noch keiner Killer vorhanden?
		if (m_killerList[FIRST_KILLER_INDEX][ply].isMoveInvalid()) return KILLER_NO_PRIO;

		// Zug = erstes Element?
		if (move == m_killerList[FIRST_KILLER_INDEX][ply]) return KILLER_PRIO_1;
		
		// Zug = zweites Element?
		if (move == m_killerList[LAST_KILLER_INDEX][ply]) return KILLER_PRIO_2;

		// Zug kein Killer?
		return KILLER_NO_PRIO;
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
}
