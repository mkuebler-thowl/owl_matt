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

		// TODO: Threshold für Random irgendwie berechnen/festlegen
		/// <summary>
		/// Funktion für die Zugfindung aus einer Spielposition heraus
		/// </summary>
		/// <param name="position:">Ausgangsstellung der Schachposition bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. nächstes am Zug ist</param>
		/// <param name="depth:">Die maximale Suchtiefe d</param>
		/// <param name="parameterFlags:">Für Min-Max-Erweiterungen können Feature-Parameter über Bitflags aktiviert werden</param>
		/// <param name="random:">Bei Aktivierung wird aus einer Menge bestmöglicher Züge (+-Threshold) ein Zug zufällig gewählt</param>
		/// <returns>Der bestmögliche gefundene Zug innerhalb der Suchtiefe</returns>
		Move searchMove(short player, UINT16 depth, UCHAR parameter_flags, BOOL random = false);

		BOOL inDebugMode() const;
		VOID setDebugMode(BOOL debug);
		VOID stop();
		BOOL isReady() const;
		VOID setOption(const std::string& option, const std::string& value);
		const EngineOptions& getOptions() const;

		Position& getPosition();
		const Position& getPosition() const;

	private:
		/// <summary>
		/// Klassischer Min-Max-Algorithmus zur Zugfindung
		/// </summary>
		/// <param name="position:">Ausgangsstellung bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. nächstes am Zug ist</param>
		/// <param name="depth:">maximale Suchtiefe d</param>
		/// <param name="sort:">Züge nach Nutzen sortieren?</param>
		/// <param name="killer">Killer-Heuristik verwenden?</param>
		/// <returns>Nutzwert</returns>
		MinMaxResult minMax(Position& position, short player, UINT16 depth);

		/// <summary>
		/// Alpha-Beta-Suche (Erweiterung des Min-Max)
		/// </summary>
		/// <param name="position">Ausgangsstellung bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. nächstes am Zug ist</param>
		/// <param name="depth:">maximale Suchtiefe d</param>
		/// <param name="alpha:">Alpha-Wert</param>
		/// <param name="beta:">Beta-Wert</param>
		/// <param name="sort:">Züge nach Nutzen sortieren?</param>
		/// <returns></returns>
		MinMaxResult alphaBeta(Position& position, short player, UINT16 depth, FLOAT alpha, FLOAT beta, UCHAR parameterFlags, KILLER_LIST* killerList = nullptr);

		//MinMaxResult nested(Position& position, short player, UINT16 depth, BOOL sort = false);
		//MinMaxResult nestedAlphaBeta(Position& position, short player, UINT16 depth, FLOAT alpha, FLOAT beta, BOOL sort = false);
	public:
		VOID sortMoves(MOVE_LIST* moves, Position& position, UINT16 depth, UCHAR parameterFlags, KILLER_LIST* killerList = nullptr);

		static VOID insertKiller(KILLER_LIST* killerList, const Move move, const UINT16 ply)
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

		static UINT16 compareKiller(KILLER_LIST* killerList, const Move move, const UINT16 ply)
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

		// temporär:
		INT32 m_v1 = 0;
		INT32 m_v2 = 0;

		BOOL m_debugMode = false;
		BOOL m_ready = true;
		BOOL m_stop = false;

		std::mutex m_mutex;
		Position m_position;
		EngineOptions m_engineOptions;
		RepitionMap m_repitionMap;
	};
}