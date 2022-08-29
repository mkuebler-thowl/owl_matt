#pragma once

#include "defines.hpp"
#include <iostream>

namespace owl
{
	/**
	 * Datenstruktur zur Information eines Spielzugs, der auf eine Spielstellung ausgeführt werden kann.
	 */
	struct Move
	{
		INT32 startX = 0;		// Startfeld (Spalte)
		INT32 startY = 0;		// Startfeld (Reihe)
		INT32 targetX = 0;		// Zielfeld (Spalte)
		INT32 targetY = 0;		// Zielfeld (Reihe)

		BOOL capture = false;	// Figure geschlagen
		CHAR promotion = 0;		// Bauernumwandlung
		BOOL enPassantCapture = false;// Figure durch EnPassant geschlagen?
		BOOL castlingLong = false; // Ist der Zug eine lange Rochade?
		BOOL castlingShort = false;// Ist der Zug eine kurze Rochade?

		/**
		 * Print-Funktion zur Ausgabe der Attribute.
		 */
		VOID print() const
		{
			std::cout << "{ start: " << startX << ", " << startY
				<< "; target: " << targetX << ", " << targetY
				<< "; capture= " << capture
				<< "; promotion= " << promotion
				<< "; enPassantCapture= " << enPassantCapture
				<< "; castlingLong= " << castlingLong
				<< "; castlingShort= " << castlingShort
				<< "}\n";
		}

		/**
		 * Ist der Zug an der gleichen Stelle?
		 * 
		 * \param other Zug der verglichen werden soll
		 * \return gleicher Zug?
		 */
		constexpr BOOL operator==(const Move& other) const
		{
			return startX == other.startX 
				&& startY == other.startY 
				&& targetX == other.targetX 
				&& targetY == other.targetY;
		}

		/**
		 * Ist der Zug nicht an der gleichen Stelle?
		 *
		 * \param other Zug der verglichen werden soll
		 * \return !(gleicher Zug)
		 */
		constexpr BOOL operator!=(const Move& other) const
		{
			return !(operator==(other));
		}

		/**
		 * Ist der Zug illegal?.
		 * 
		 * \return Ist der Zug illegal?
		 */
		BOOL isMoveInvalid() const
		{
			return startX == targetX
				&& startY == targetY;
		}

		/**
		 * Hashfunktions-Struct. Zur Verwendung der std::unordered_map<Move,...>
		 */
		struct HashFunction
		{
			/**
			 * Die Hash-Operation zur Unterscheidung der Züge innerhalb der std::unordered_map<Move,...>
			 * 
			 * \param move Der Zug der gehasht werden soll
			 * \return Der Hashwert
			 */
			UINT64 operator()(const Move& move) const
			{
				UINT64 s_x = std::hash<INT32>()(move.startX);
				UINT64 s_y = std::hash<INT32>()(move.startY) << 1;
				UINT64 t_x = std::hash<INT32>()(move.targetX) << 2;
				UINT64 t_y = std::hash<INT32>()(move.targetY) << 3;
				return (s_x ^ s_y ^ t_x ^ t_y);
			}
		};
	};

	constexpr Move INVALID_MOVE = { 0,0,0,0 }; // Initialzug (Illegal)
}