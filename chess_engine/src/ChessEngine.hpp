#pragma once

#include <vector>
#include "defines.hpp"

#include "Position.hpp"
#include "Move.hpp"
#include "MinMaxResult.hpp"
#include "ChessValidation.hpp"
#include "RepitionMap.hpp"

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
		Move searchMove(Position& position, short player, UINT16 depth, UCHAR parameter_flags, BOOL random = false);
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
		MinMaxResult alphaBeta(Position& position, short player, UINT16 depth, FLOAT alpha, FLOAT beta, BOOL sort = false, BOOL killer = false);
	
		MinMaxResult nested(Position& position, short player, UINT16 depth, BOOL sort = false);

		MinMaxResult nestedAlphaBeta(Position& position, short player, UINT16 depth, FLOAT alpha, FLOAT beta, BOOL sort = false);
	public:
		VOID sortMoves(MoveList* moves, Position& position, UINT16 depth, MinMaxResult* pResult, BOOL killer = false);

		static BOOL killerHeuristics(const Move& move, UINT16 depth, MinMaxResult* pResult);

		enum class Captures
		{
			kxP,kxN,kxB,kxR,kxQ,
			qxP,qxN,qxB,qxR,qxQ,
			rxP,rxN,rxB,rxR,rxQ,
			bxP,bxN,bxB,bxR,bxQ,
			nxP,nxN,nxB,nxR,nxQ,
			pxP,pxN,pxB,pxR,pxQ
		};
	
		static constexpr std::array<CHAR*, MAX_PIECE_TYPES*(MAX_PIECE_TYPES-1)> s_capture_map =
		{
			"kxP","kxN","kxB","kxR","kxQ",
			"qxP","qxN","qxB","qxR","qxQ",
			"rxP","rxN","rxB","rxR","rxQ",
			"bxP","bxN","bxB","bxR","bxQ",
			"nxP","nxN","nxB","nxR","nxQ",
			"pxP","pxN","pxB","pxR","pxQ"
		};

		static Captures getCaptureValue(CHAR attacker, CHAR victim);
		static std::string getCaptureValueString(Captures capture);
	

		INT32 m_v1 = 0;
		INT32 m_v2 = 0;

		RepitionMap m_repitionMap;
	};
}