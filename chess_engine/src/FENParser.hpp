#pragma once

#include <string>

#include "Position.hpp"
#include "Move.hpp"

namespace matt
{
	/// FENParser ist eine Hilfsklasse zur Umwandlung von FEN-Strings in Positionen, Algebraische Notationen in Züge und umgekehrt.
	class FENParser
	{
	public:
		/// <summary>
		/// Einen FEN-String in eine Position parsen.
		/// </summary>
		/// <param name="fen">FEN-String</param>
		/// <returns>Die resultierende Position</returns>
		static Position fenToPosition(const std::string& fen);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		static std::string positionToFen(const Position& pos);
		static Move stringToMove(const std::string& algebraicNotation);
		static std::string moveToString(const Move& move);
	};
}