#pragma once
#include <string>
#include <vector>
#include <map>

#include "defines.hpp"
#include "Move.hpp"
#include "Position.hpp"

namespace owl
{
	/// ChessValidation bietet als Hilfsklasse f�r statische Funktionen zur �berpr�fung und Ermittlung legaler Z�ge.
	class ChessValidation
	{
	public:
		static VOID init();

		/// <summary>
		/// Funktion zur Ermittlung g�ltiger Z�ge "Moves".
		/// </summary>
		/// <param name="position:">Aktuelle Spielstellung bzw. Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler, der am Zug ist.</param>
		/// <returns>Liste der m�glichen bzw. g�ltigen Z�ge</returns>
		static MOVE_LIST getValidMoves(Position& position, short player);

		/// <summary>
		/// Funktion zur Ermittlung der Anzahl g�ltiger Z�ge zu einer bestimmten Position. Wird f�r die Piece-Mobilty verwendet.
		/// </summary>
		/// <param name="position">Ausgangsstellung</param>
		/// <param name="x">Spalte 0-7 (rechts-links)</param>
		/// <param name="y">Zeile 0-7 (oben-unten)</param>
		/// <returns>Anzahl gefundener Z�ge</returns>
		static UINT16 countPossibleMovesOnField(Position& position, INT32 x, INT32 y);
		/// <summary>
		/// �berpr�fung, ob der K�nig durch den n�chsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der K�nig im Schach stehen w�rde</returns>
		static BOOL isKingInCheckAfterMove(Position& position, short player, const Move& move);

		/// <summary>
		/// Funktion zur �berpr�fung, ob die aktuelle Stellung den jeweiligen K�nig bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der K�nig des aktuellen Spielers im Schach?</returns>
		static BOOL isKingInCheck(Position& position, short player);

		/// �berpr�fe ob die Stelle x,y die Spielfeldgrenzen �berschreitet
		static BOOL isInsideChessboard(INT32 x, INT32 y);
	private: 
		static MOVE_LIST s_data;
		/// <summary>
		/// Ist die Platzierung x,y im Schach bzw. wird diese Position bedroht? 
		/// </summary>
		/// <param name="position">Ausgangsposition bzw. Stellung</param>
		/// <param name="x">Spalte</param>
		/// <param name="y">Reihe</param>
		/// <param name="player">Der Spieler f�r den die Position auf "Schach" �berpr�ft werden soll</param>
		/// <returns></returns>
		static BOOL isPlaceInCheck(const Position& position, INT32 x, INT32 y, short player);

		/// �berpr�fe alle diagonalen Felder vom K�nig ausgesehen
		static BOOL checkKingDiagonal(INT32 king_x, INT32 king_y, const Position& position, short player);
		/// �berpr�fe alle horizontalen und vertikalen Felder vom K�nig ausgesehen
		static BOOL checkKingAxis(INT32 king_x, INT32 king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Springerbedrohungen vom K�nig ausgesehen
		static BOOL checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Bauerbedrohungen vom K�nig ausgesehen
		static BOOL checkKingPawns(INT32 king_x, INT32 king_y, const Position& position, short player);

		/// Schachmatt �berpr�fen
		static VOID evaluateCheckmate(Position& position, short player, BOOL noValidMoves);

		/// Suche alle legalen Bauerbewegungen
		static VOID getValidPawnMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen Springerbewegungen
		static VOID getValidKnightMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen K�nigbewegungen
		static VOID getValidKingMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen Turmbewegungen
		static VOID getValidRookMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen L�uferbewegungen
		static VOID getValidBishopMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle Linien ab f�r L�ufer, T�rme und Dame
		static VOID continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir);

		/// Enemy-String nach Spielertyp ausgeben
		static std::string getEnemyPiecesString(short player);
	};
}