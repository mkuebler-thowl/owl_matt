#pragma once

#include "defines.hpp"
#include <array>
#include "Position.hpp"

namespace owl
{
	class ChessEvaluation
	{
	public:
		/// <summary>
		/// Bewertungsfunktion
		/// </summary>
		/// <param name="position">Ausgangstellung bzw. Position, die bewertet werden soll</param>
		/// <param name="enginePlayer">Der Spieler (Weiß/Schwarz), welcher von der Schach-Engine gespielt wird</param>
		/// <param name="evaluationFeatureFlags">Feature-Parameter-Flags für die Komponenten der Bewertungsfunktion</param>
		/// <returns>Nutzwert der Bewertungsfunktion</returns>
		static FLOAT evaluate(Position& position, short enginePlayer, UCHAR evaluationFeatureFlags = 0);
	private:
		/// Doppelte Bauern?
		static BOOL isDoublePawn(const Position& position, INT32 x, INT32 y);
		/// Verbundene Bauern?
		static BOOL isConnectedPawn(const Position& position, INT32 x, INT32 y);
		/// Backwards Bauern?
		static BOOL isBackwardsPawn(const Position& position, INT32 x, INT32 y);
		/// Passierte Bauern?
		static BOOL isPassedPawn(const Position& position, INT32 x, INT32 y);
		/// Verkettete Bauern? 
		static BOOL isChainPawn(const Position& position, INT32 x, INT32 y);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein freundlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static BOOL isPieceEqualOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein feindlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static BOOL isPieceEnemyPawnOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset);
	public:

		static UCHAR GetPlayerIndexByPositionPlayer(short currentPlayerOfPosition);
		static UCHAR GetEnemyPiece(short currentPlayerOfPosition, UINT16 pieceIndex);
	};
}