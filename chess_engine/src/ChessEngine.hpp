#pragma once

#include <vector>
#include "defines.hpp"

#include "Position.hpp"
#include "Move.hpp"
#include "MinMaxResult.hpp"
#include "ChessValidation.hpp"
#include "RepitionMap.hpp"
#include "EngineOptions.hpp"

#include <mutex>

namespace owl
{
	/// Schach-Engine-Klasse. Beinhaltet die Logik einer Schach-Engine, wie die Zugfindung und Bewertung eines Zugs usw.
	class ChessEngine
	{
	public:
		explicit ChessEngine();
		virtual ~ChessEngine();

		/// <summary>
		/// Funktion für die Zugfindung aus einer Spielposition heraus
		/// </summary>
		/// <param name="position:">Ausgangsstellung der Schachposition bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. nächstes am Zug ist</param>
		/// <param name="depth:">Die maximale Suchtiefe d</param>
		/// <param name="parameterFlags:">Für Min-Max-Erweiterungen können Feature-Parameter über Bitflags aktiviert werden</param>
		/// <param name="random:">Bei Aktivierung wird aus einer Menge bestmöglicher Züge (+-Threshold) ein Zug zufällig gewählt</param>
		/// <returns>Der bestmögliche gefundene Zug innerhalb der Suchtiefe</returns>
		PAIR<Move, EVALUATION_VALUE> searchMove(INT32 player, INT32 depth, UCHAR parameter_flags);

		BOOL inDebugMode() const;
		VOID setDebugMode(BOOL debug);
		VOID stop();
		BOOL isReady() const;
		VOID setOption(const std::string& option, const std::string& value);
		const EngineOptions& getOptions() const;

		VOID setPosition(const Position& position);

		Position& getPosition();
		const Position& getPosition() const;

		INT32 getPlayer() const;
	private:
		EVALUATION_VALUE minMax(Position& position, INT32 player, INT32 depth);
		EVALUATION_VALUE alphaBeta(Position& position, INT32 player,
			INT32 depth, FLOAT alpha, FLOAT beta, 
			UCHAR parameterFlags, KILLER_LIST* killerList = nullptr);
	public:
		VOID sortMoves(MOVE_LIST* moves, Position& position, INT32 depth, 
			UCHAR parameterFlags, KILLER_LIST* killerList = nullptr);
		static VOID insertKiller(KILLER_LIST* killerList, const Move move, const INT32 ply)
		{
			 if (killerList == nullptr || ply >= MAX_DEPTH) return;
			 if (move.capture) return;

			auto killer_list = *killerList;

			if (killer_list[FIRST_KILLER_INDEX][ply] != move
				&& killer_list[LAST_KILLER_INDEX][ply] != move)
			{
				// shift first item
				 killer_list[LAST_KILLER_INDEX][ply] = killer_list[FIRST_KILLER_INDEX][ply];

				 // insert new item
				 killer_list[FIRST_KILLER_INDEX][ply] = move;
			}
		}
		static INT32 compareKiller(KILLER_LIST* killerList, const Move move, const INT32 ply)
		{
			auto first_killer = *(killerList[FIRST_KILLER_INDEX][ply]);
			if (first_killer.isMoveInvalid()) return KILLER_NO_PRIO;

			auto last_killer = *(killerList[LAST_KILLER_INDEX][ply]);

			if (move == first_killer) return KILLER_PRIO_1;
			else if (move == last_killer) return KILLER_PRIO_2;
			return KILLER_NO_PRIO;
		}

		static Captures getCaptureValue(CHAR attacker, CHAR victim);
		static std::string getCaptureValueString(Captures capture);

		BOOL m_debugMode = false;
		BOOL m_ready = true;
		BOOL m_stop = false;
		INT32 m_startedDepth = 0;
        
		std::mutex m_mutex;
		INT32 m_player;
		Position m_position;
		EngineOptions m_engineOptions;
		RepitionMap m_repitionMap;
		MinMaxResult m_result;
	};
}
