#pragma once

#include "defines.hpp"
#include <array>
#include "Position.hpp"

namespace owl
{
	/**
	 * Klasse zur Ermittlung der Bewertungsfunktion durch Heuristiken.
	 */
	class ChessEvaluation
	{
	public:
		/**
		 * Bewertungsfunktion
		 * 
		 * \param position Ausgangstellung bzw. Position, die bewertet werden soll
		 * \param enginePlayer Der Spieler (Wei�/Schwarz), welcher von der Schach-Engine gespielt wird
		 * \param evaluationFeatureFlags Feature-Parameter-Flags f�r die Komponenten der Bewertungsfunktion, die mit EVAL_FT_* beginnen
		 * \param validationCheckMate Ob Checkmate �berpr�fung stattfinden sol
		 * \return Nutzwert der Bewertungsfunktion
		 */
		static EVALUATION_VALUE evaluate(Position& position, INT32 enginePlayer, UCHAR evaluationFeatureFlags = 0, BOOL validationCheckMate = false);
	private:
		/**
		 * Doppelte Bauernstruktur gefunden?
		 * 
		 * \param position Ausgangsstellung
		 * \param x Spalte
		 * \param y Reihe
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isDoublePawn(const Position& position, INT32 x, INT32 y);
		/**
		 * Verbunden Bauernstruktur gefunden?
		 *
		 * \param position Ausgangsstellung
		 * \param x Spalte
		 * \param y Reihe
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isConnectedPawn(const Position& position, INT32 x, INT32 y);
		/**
		 * R�ckl�ufige Bauernstruktur gefunden?
		 *
		 * \param position Ausgangsstellung
		 * \param x Spalte
		 * \param y Reihe
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isBackwardsPawn(const Position& position, INT32 x, INT32 y);
		/**
		 * Freiebauern gefunden?
		 *
		 * \param position Ausgangsstellung
		 * \param x Spalte
		 * \param y Reihe
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isPassedPawn(const Position& position, INT32 x, INT32 y);
		/**
		 * Verkettete Bauernstruktur gefunden?
		 *
		 * \param position Ausgangsstellung
		 * \param x Spalte
		 * \param y Reihe
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isChainPawn(const Position& position, INT32 x, INT32 y);
		/**
		 * Hilfsfunktion zur �berpr�fung, ob sich ein freundlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param xOffset Offset f�r die Spalte
		 * \param yOffset Offsest f�r die Zeile
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isPawnPieceAlliedOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset);
		/**
		 * Hilfsfunktion zur �berpr�fung, ob sich ein feindlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param xOffset Offset f�r die Spalte
		 * \param yOffset Offset f�r die Zeile
		 * \return Eigenschaft zutreffend?
		 */
		static BOOL isPawnPieceHostileOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset);
	public:
		/**
		 * Gebe den Spielerindex zum korrespondierenden aktuellen Spieler zur�ck
		 * 
		 * \param currentPlayerOfPosition PLAYER_WHITE oder PLAYER_BLACK
		 * \return WHITE_INDEX, BLACK_INDEX
		 */
		static INT32 GetPlayerIndexByPositionPlayer(INT32 currentPlayerOfPosition);
		/**
		 * Gebe die gegnerische �qivalente Figur einer Figur zur�ck
		 * 
		 * \param currentPlayerOfPosition Aktueller Spieler der Position
		 * \param pieceIndex Index des Figurentyps
		 * \return Figur
		 */
		static UCHAR GetEnemyPiece(INT32 currentPlayerOfPosition, INT32 pieceIndex);
	};
}