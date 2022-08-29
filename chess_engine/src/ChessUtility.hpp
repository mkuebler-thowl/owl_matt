#pragma once

#include <string>

#include "defines.hpp"
#include "Position.hpp"
#include "Move.hpp"

namespace owl
{
	/**
	 * Hilfsklasse zur Umwandlung von FEN-Strings in Positionen, Algebraische Notation in Züge und umgekehrt.
	 */
	class ChessUtility
	{
	public:
		/**
		 * Einen FEN-String in eine Position parsen.
		 * 
		 * \param fen FEN-String
		 * \return Generierte Position
		 */
		static Position fenToPosition(std::string fen);
		/**
		 * FEN-String aus einer Position generieren.
		 * 
		 * \param pos Position
		 * \return Generierter FEN-String
		 */
		static std::string positionToFen(const Position& pos);
		/**
		 * Eine Algebraische Notation in einen Move parsen.
		 * 
		 * \param algebraicNotation Der Zug in Schreibweise der algebraischen Notation
		 * \param pos die aktuelle Position (Wichtig für Bestimmung von Rochade, En Passant usw.)
		 * \return Der generierte Zug
		 */
		static Move stringToMove(const std::string& algebraicNotation, const Position* pos = nullptr);
		/**
		 * Ein String in der algebraischen Notation aus einem Zug-Objekt generieren.
		 * 
		 * \param move Zug-Objekt
		 * \return Generierter String
		 */
		static std::string moveToString(const Move& move);
		/**
		 * Berechne einen Wert in Centipawns bzw. einfach in Hundertstel.
		 * 
		 * \param VALUE Der eingegebene Wert
		 * \return Das Hunderstel des Werts
		 */
		static INT32 convertToCentipawns(FLOAT VALUE);
	};
}