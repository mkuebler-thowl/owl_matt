#pragma once
#include <string>
#include <vector>
#include <map>

#include "defines.hpp"
#include "Move.hpp"
#include "Position.hpp"

namespace owl
{
	/**
	 * ChessValidation bietet als Hilfsklasse f�r statische Funktionen zur �berpr�fung und Ermittlung legaler Z�ge.
	 */
	class ChessValidation
	{
	public:
		static VOID init();
		/**
		 * Funktion zur Ermittlung g�ltiger Z�ge "Moves".
		 * 
		 * \param position Aktuelle Spielstellung bzw. Ausgangsstellung
		 * \param player Aktueller Spieler, der am Zug ist
		 * \return Liste der m�glichen bzw. g�ltigen Z�ge
		 */
		static MOVE_LIST getValidMoves(Position& position, INT32 player);
		/**
		 * Funktion zur Ermittlung der Anzahl g�ltiger Z�ge zu einer bestimmten Position. Wird f�r die Piece-Mobilty verwendet.
		 * 
		 * \param position Ausgangsstellung
		 * \param x Spalte 0-7 (rechts-links)
		 * \param y Zeile 0-7 (oben-unten)
		 * \param noKingCheck Ob "Im Schach"-Z�ge nicht ber�cksichtigt werden sollen
		 * \return Anzahl gefundener Z�ge
		 */
		static INT32 countPossibleMovesOnField(Position& position, INT32 x, INT32 y, BOOL noKingCheck = false);
		/**
		 * �berpr�fung, ob der K�nig durch den n�chsten Zug im Schach steht.
		 * 
		 * \param position Ausgangsstellung
		 * \param player Aktueller Spieler
		 * \param move Angewendeter Zug
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 * \return Anzahl der legalen Z�ge vom Feld auf x,y
		 */
		static BOOL isKingInCheckAfterMove(Position& position, INT32 player, const Move& move, BOOL noKingCheck = false);
		/**
		 * Ist der K�nig im Schach?
		 * 
		 * \param position Stellung
		 * \param player Spieler des K�nigs
		 * \param noKingCheck Ob "Im Schach"-Z�ge nicht ber�cksichtigt werden sollen
		 * \return Zutreffend?
		 */
		static BOOL isKingInCheck(Position& position, INT32 player, BOOL noKingCheck = false);
		/**
		 * Ist die Position x,y innerhalb des Schachfelds?
		 * 
		 * \param x Spalte
		 * \param y Zeile
		 * \return Zutreffend?
		 */
		static BOOL isInsideChessboard(INT32 x, INT32 y);
	private: 
		static MOVE_LIST s_data; // Liste f�r die Zwischenspeicherung der validen Z�ge
		/**
		 * Ist die Platzierung von x und y "im Schach" bzw. wird diese Position bedroht? 
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler der �berpr�ft werden soll
		 * \param Ob "Im Schach"-Z�ge nicht ber�cksichtigt werden sollen 
		 */
		static BOOL isPlaceInCheck(const Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		 /**
		  * �berpr�fe alle diagonalen Felder vom K�nig ausgesehen
		  *
		  * \param king_x Spalte des K�nigs
		  * \param king_y Zeile des K�nigs
		  * \param position Stellung
		  * \param player Spieler der �berpr�ft werden soll
		  */
		static BOOL checkKingDiagonal(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * �berpr�fe alle horizontalen und vertikalen Felder vom K�nig ausgesehen
		  *
		  * \param king_x Spalte des K�nigs
		  * \param king_y Zeile des K�nigs
		  * \param position Stellung
		  * \param player Spieler der �berpr�ft werden soll
		  */
		static BOOL checkKingAxis(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * �berpr�fe alle m�glichen Springerbedrohungen vom K�nig ausgesehen
		  *
		  * \param king_x Spalte des K�nigs
		  * \param king_y Zeile des K�nigs
		  * \param position Stellung
		  * \param player Spieler der �berpr�ft werden soll
		  */
		static BOOL checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * �berpr�fe alle m�glichen Bauerbedrohungen vom K�nig ausgesehen
		  *
		  * \param king_x Spalte des K�nigs
		  * \param king_y Zeile des K�nigs
		  * \param position Stellung
		  * \param player Spieler der �berpr�ft werden soll
		  */
		static BOOL checkKingPawns(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		 * �berpr�fe die Position auf ein "Schachmatt".
		 * 
		 * \param position Stellung
		 * \param player der Spieler, der "im Schach" steht
		 * \param noValidMoves Ob legale Z�ge gefunden wurden
		 */
		static VOID evaluateCheckmate(Position& position, INT32 player, BOOL noValidMoves);
		/**
		 * Suche alle legalen Bauerbewegungen.
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID getValidPawnMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 * Suche alle legalen Springerbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID getValidKnightMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen K�nigbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID getValidKingMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen Turmbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID getValidRookMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen L�uferbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID getValidBishopMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 * Suche alle legalen Z�ge auf einer Linie ab. (F�r L�ufer, T�rme und Dame)
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param enemies_string Gegnerische Figurenstring
		 * \param xDir x-Richtung
		 * \param yDir y-Richtung
		 * \param noKingCheck Ob der K�nig nach dem Zug "im Schach" stehen w�rde
		 */
		static VOID continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir, BOOL noKingCheck = false);
		/**
		 * Lese den gegnerischen String des jeweiligen Spielers aus.
		 * 
		 * \param player Der Spieler zu dem der gegnerische Figurenstring ausgelesen wird
		 * \return Gegnerischer Figurenstring
		 */
		static std::string getEnemyPiecesString(INT32 player);
	};
}