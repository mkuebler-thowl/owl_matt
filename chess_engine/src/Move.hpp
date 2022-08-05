#pragma once
#include <iostream>

namespace matt
{
	/// Datenstruktur zur Information eines Spielzugs, der auf eine Spielstellung ausgeführt werden kann.
	struct Move
	{
		/// Startfeld (Spalte)
		unsigned short startX = 0;
		/// Startfeld (Reihe)
		unsigned short startY = 0;

		/// Zielfeld (Spalte)
		unsigned short targetX = 0;
		/// Zielfeld (Reihe)
		unsigned short targetY = 0;

		/// Figure geschlagen
		bool capture = false;
		/// Bauernumwandlung
		char promotion = 0;

		/// <summary>
		/// Figure durch EnPassant geschlagen?
		/// </summary>
		bool enPassantCapture = false;
		
		/// <summary>
		/// Ist der Zug eine lange Rochade?
		/// </summary>
		bool castlingLong = false;

		/// <summary>
		/// Ist der Zug eine kurze Rochade?
		/// </summary>
		bool castlingShort = false;

		static void printMove(const Move& move)
		{
			std::cout << "{ start: " << move.startX << ", " << move.startY
				<< "; target: " << move.targetX << ", " << move.targetY
				<< "; capture= " << move.capture
				<< "; promotion= " << move.promotion
				<< "; enPassantCapture= " << move.enPassantCapture
				<< "; castlingLong= " << move.castlingLong
				<< "; castlingShort= " << move.castlingShort
				<< "}\n";
		}
	};
}