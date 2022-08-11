#pragma once

#include "defines.hpp"
#include <iostream>

namespace owl
{

	/// Datenstruktur zur Information eines Spielzugs, der auf eine Spielstellung ausgeführt werden kann.
	struct Move
	{
		/// Startfeld (Spalte)
		UINT16 startX = 0;
		/// Startfeld (Reihe)
		UINT16 startY = 0;

		/// Zielfeld (Spalte)
		UINT16 targetX = 0;
		/// Zielfeld (Reihe)
		UINT16 targetY = 0;

		/// Figure geschlagen
		BOOL capture = false;
		/// Bauernumwandlung
		CHAR promotion = 0;

		/// <summary>
		/// Figure durch EnPassant geschlagen?
		/// </summary>
		BOOL enPassantCapture = false;
		
		/// <summary>
		/// Ist der Zug eine lange Rochade?
		/// </summary>
		BOOL castlingLong = false;

		/// <summary>
		/// Ist der Zug eine kurze Rochade?
		/// </summary>
		BOOL castlingShort = false;

		static VOID printMove(const Move& move)
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

		/// Ist der Zug an der gleichen Stelle?
		constexpr BOOL operator==(const Move& other) const
		{
			return startX == other.startX 
				&& startY == other.startY 
				&& targetX == other.targetX 
				&& targetY == other.targetY;
		}

		/// Ist der Zug nicht an der gleichen Stelle?
		constexpr BOOL operator!=(const Move& other) const
		{
			return !(operator==(other));
		}

		struct HashFunction
		{
			UINT64 operator()(const Move& move) const
			{
				UINT64 s_x = std::hash<UINT16>()(move.startX);
				UINT64 s_y = std::hash<UINT16>()(move.startY) << 1;
				UINT64 t_x = std::hash<UINT16>()(move.targetX) << 2;
				UINT64 t_y = std::hash<UINT16>()(move.targetY) << 3;
				return (s_x ^ s_y ^ t_x ^ t_y);
			}
		};
	};
}