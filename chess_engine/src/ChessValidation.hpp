#pragma once
#include <string>
#include <vector>
#include <map>

#include "defines.hpp"
#include "Move.hpp"
#include "Position.hpp"

namespace owl
{
	/// ChessValidation bietet als Hilfsklasse für statische Funktionen zur Überprüfung und Ermittlung legaler Züge.
	class ChessValidation
	{
	public:
		static VOID init();

		/// <summary>
		/// Funktion zur Ermittlung gültiger Züge "Moves".
		/// </summary>
		/// <param name="position:">Aktuelle Spielstellung bzw. Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler, der am Zug ist.</param>
		/// <returns>Liste der möglichen bzw. gültigen Züge</returns>
		static MOVE_LIST getValidMoves(Position& position, INT32 player);

		/// <summary>
		/// Funktion zur Ermittlung der Anzahl gültiger Züge zu einer bestimmten Position. Wird für die Piece-Mobilty verwendet.
		/// </summary>
		/// <param name="position">Ausgangsstellung</param>
		/// <param name="x">Spalte 0-7 (rechts-links)</param>
		/// <param name="y">Zeile 0-7 (oben-unten)</param>
		/// <returns>Anzahl gefundener Züge</returns>
		static INT32 countPossibleMovesOnField(Position& position, INT32 x, INT32 y, BOOL noKingCheck = false);
		/// <summary>
		/// Überprüfung, ob der König durch den nächsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der König im Schach stehen würde</returns>
		static BOOL isKingInCheckAfterMove(Position& position, INT32 player, const Move& move, BOOL noKingCheck = false);

		/// <summary>
		/// Funktion zur Überprüfung, ob die aktuelle Stellung den jeweiligen König bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der König des aktuellen Spielers im Schach?</returns>
		static BOOL isKingInCheck(Position& position, INT32 player, BOOL noKingCheck = false);

		/// Überprüfe ob die Stelle x,y die Spielfeldgrenzen überschreitet
		static BOOL isInsideChessboard(INT32 x, INT32 y);
	private: 
		static MOVE_LIST s_data;
		/// <summary>
		/// Ist die Platzierung x,y im Schach bzw. wird diese Position bedroht? 
		/// </summary>
		/// <param name="position">Ausgangsposition bzw. Stellung</param>
		/// <param name="x">Spalte</param>
		/// <param name="y">Reihe</param>
		/// <param name="player">Der Spieler für den die Position auf "Schach" überprüft werden soll</param>
		/// <returns></returns>
		static BOOL isPlaceInCheck(const Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);

		/// Überprüfe alle diagonalen Felder vom König ausgesehen
		static BOOL checkKingDiagonal(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/// Überprüfe alle horizontalen und vertikalen Felder vom König ausgesehen
		static BOOL checkKingAxis(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/// Überprüfe alle möglichen Springerbedrohungen vom König ausgesehen
		static BOOL checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, INT32 player);
		/// Überprüfe alle möglichen Bauerbedrohungen vom König ausgesehen
		static BOOL checkKingPawns(INT32 king_x, INT32 king_y, const Position& position, INT32 player);

		/// Schachmatt überprüfen
		static VOID evaluateCheckmate(Position& position, INT32 player, BOOL noValidMoves);

		/// Suche alle legalen Bauerbewegungen
		static VOID getValidPawnMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/// Suche alle legalen Springerbewegungen
		static VOID getValidKnightMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/// Suche alle legalen Königbewegungen
		static VOID getValidKingMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/// Suche alle legalen Turmbewegungen
		static VOID getValidRookMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);
		/// Suche alle legalen Läuferbewegungen
		static VOID getValidBishopMoves(Position& position, INT32 x, INT32 y, INT32 player, BOOL noKingCheck = false);

		/// Suche alle Linien ab für Läufer, Türme und Dame
		static VOID continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir, BOOL noKingCheck = false);

		/// Enemy-String nach Spielertyp ausgeben
		static std::string getEnemyPiecesString(INT32 player);
	};
}