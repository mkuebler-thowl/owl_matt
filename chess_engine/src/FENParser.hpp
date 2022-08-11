#pragma once

#include <string>

#include "Position.hpp"
#include "Move.hpp"

namespace owl
{
	/// FENParser ist eine Hilfsklasse zur Umwandlung von FEN-Strings in Positionen, Algebraische Notationen in Z�ge und umgekehrt.
	class FENParser
	{
	public:
		/// <summary>
		/// Einen FEN-String in eine Position parsen.
		/// </summary>
		/// <param name="fen">FEN-String</param>
		/// <returns>Die resultierende Position</returns>
		static Position fenToPosition(std::string fen);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		static std::string positionToFen(const Position& pos);

		/// <summary>
		/// Eine Algebraische Notation in einen Move parsen.
		/// </summary>
		/// <param name="algebraicNotation:">Notation</param>
		/// <returns>Move-Objekt (Zug)</returns>
		static Move stringToMove(const std::string& algebraicNotation);
		/// <summary>
		/// Ein Move-Objekt (Zug) in die Algebraische Notation umwandeln.
		/// </summary>
		/// <param name="move:">Das Move-Objekt (Der Zug)</param>
		/// <returns>String in der algebraischen Notation</returns>
		static std::string moveToString(const Move& move);
	};
}