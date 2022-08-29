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
	 * ChessValidation bietet als Hilfsklasse für statische Funktionen zur Überprüfung und Ermittlung legaler Züge.
	 */
	class ChessValidation
	{
	public:
		static VOID init();
		/**
		 * Funktion zur Ermittlung gültiger Züge "Moves".
		 * 
		 * \param position Aktuelle Spielstellung bzw. Ausgangsstellung
		 * \param player Aktueller Spieler, der am Zug ist
		 * \return Liste der möglichen bzw. gültigen Züge
		 */
		static MOVE_LIST getValidMoves(Position& position, INT32 player);
		/**
		 * Funktion zur Ermittlung der Anzahl gültiger Züge zu einer bestimmten Position. Wird für die Piece-Mobilty verwendet.
		 * 
		 * \param position Ausgangsstellung
		 * \param x Spalte 0-7 (rechts-links)
		 * \param y Zeile 0-7 (oben-unten)
		 * \param noKingCheck Ob "Im Schach"-Züge nicht berücksichtigt werden sollen
		 * \return Anzahl gefundener Züge
		 */
		static INT32 countPossibleMovesOnField(Position& position, INT32 x, INT32 y, BOOL noKingCheck = false);
		/**
		 * Überprüfung, ob der König durch den nächsten Zug im Schach steht.
		 * 
		 * \param position Ausgangsstellung
		 * \param player Aktueller Spieler
		 * \param move Angewendeter Zug
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 * \return Anzahl der legalen Züge vom Feld auf x,y
		 */
		static BOOL isKingInCheckAfterMove(Position& position, INT32 player, const Move& move, BOOL noKingCheck = false);
		/**
		 * Ist der König im Schach?
		 * 
		 * \param position Stellung
		 * \param player Spieler des Königs
		 * \param noKingCheck Ob "Im Schach"-Züge nicht berücksichtigt werden sollen
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
		static MOVE_LIST s_data; // Liste für die Zwischenspeicherung der validen Züge
		/**
		 * Ist die Platzierung von x und y "im Schach" bzw. wird diese Position bedroht? 
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler der überprüft werden soll
		 * \param Ob "Im Schach"-Züge nicht berücksichtigt werden sollen 
		 */
		static BOOL isPlaceInCheck(const Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		 /**
		  * Überprüfe alle diagonalen Felder vom König ausgesehen
		  *
		  * \param king_x Spalte des Königs
		  * \param king_y Zeile des Königs
		  * \param position Stellung
		  * \param player Spieler der überprüft werden soll
		  */
		static BOOL checkKingDiagonal(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * Überprüfe alle horizontalen und vertikalen Felder vom König ausgesehen
		  *
		  * \param king_x Spalte des Königs
		  * \param king_y Zeile des Königs
		  * \param position Stellung
		  * \param player Spieler der überprüft werden soll
		  */
		static BOOL checkKingAxis(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * Überprüfe alle möglichen Springerbedrohungen vom König ausgesehen
		  *
		  * \param king_x Spalte des Königs
		  * \param king_y Zeile des Königs
		  * \param position Stellung
		  * \param player Spieler der überprüft werden soll
		  */
		static BOOL checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		  * Überprüfe alle möglichen Bauerbedrohungen vom König ausgesehen
		  *
		  * \param king_x Spalte des Königs
		  * \param king_y Zeile des Königs
		  * \param position Stellung
		  * \param player Spieler der überprüft werden soll
		  */
		static BOOL checkKingPawns(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/**
		 * Überprüfe die Position auf ein "Schachmatt".
		 * 
		 * \param position Stellung
		 * \param player der Spieler, der "im Schach" steht
		 * \param noValidMoves Ob legale Züge gefunden wurden
		 */
		static VOID evaluateCheckmate(Position& position, INT32 player, BOOL noValidMoves);
		/**
		 * Suche alle legalen Bauerbewegungen.
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 */
		static VOID getValidPawnMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 * Suche alle legalen Springerbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 */
		static VOID getValidKnightMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen Königbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 */
		static VOID getValidKingMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen Turmbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 */
		static VOID getValidRookMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 *  Suche alle legalen Läuferbewegungen
		 *
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param player Spieler
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
		 */
		static VOID getValidBishopMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/**
		 * Suche alle legalen Züge auf einer Linie ab. (Für Läufer, Türme und Dame)
		 * 
		 * \param position Stellung
		 * \param x Spalte
		 * \param y Zeile
		 * \param enemies_string Gegnerische Figurenstring
		 * \param xDir x-Richtung
		 * \param yDir y-Richtung
		 * \param noKingCheck Ob der König nach dem Zug "im Schach" stehen würde
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