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
		/// <summary>
		/// Funktion zur Ermittlung g�ltiger Z�ge "Moves".
		/// </summary>
		/// <param name="position:">Aktuelle Spielstellung bzw. Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler, der am Zug ist.</param>
		/// <returns>Liste der m�glichen bzw. g�ltigen Z�ge</returns>
		static MoveList getValidMoves(Position& position, short player);

		/// <summary>
		/// Funktion zur Ermittlung der Anzahl g�ltiger Z�ge zu einer bestimmten Position. Wird f�r die Piece-Mobilty verwendet.
		/// </summary>
		/// <param name="position">Ausgangsstellung</param>
		/// <param name="x">Spalte 0-7 (rechts-links)</param>
		/// <param name="y">Zeile 0-7 (oben-unten)</param>
		/// <returns>Anzahl gefundener Z�ge</returns>
		static UINT16 countPossibleMovesOnField(Position& position, INT32 x, INT32 y);

		/// <summary>
		/// Zug auf eine Position anwenden. Hinweis: Diese Funktion �berpr�ft nicht, ob der Zug auf die Position angwendet werden darf. In der Zugfindung sp�ter wichtig, in der �berpr�fungphase irrelevant.
		/// </summary>
		/// <param name="position:">Aktuelle Spielposition bzw. Ausgangsstellung</param>
		/// <param name="move">Der Zug, der ausgef�hrt werden soll</param>
		/// <returns>die neue Spielposition bzw. Ausgangsstellung</returns>
		static Position applyMove(Position& position, const Move& move);

		// TODO: position.getPlayer() macht player:parameter �berfl�ssig?
		/// <summary>
		/// �berpr�fung, ob der K�nig durch den n�chsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der K�nig im Schach stehen w�rde</returns>
		static BOOL isKinginCheckAfterMove(Position& position, short player, const Move& move);

		/// �berpr�fe ob die Stelle x,y die Spielfeldgrenzen �berschreitet
		static BOOL isInsideChessboard(INT32 x, INT32 y);
	private:

		/// <summary>
		/// Funktion zur �berpr�fung, ob die aktuelle Stellung den jeweiligen K�nig bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der K�nig des aktuellen Spielers im Schach?</returns>
		static BOOL isKingInCheck(Position& position, short player);

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
		static MoveList getValidPawnMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen Springerbewegungen
		static MoveList getValidKnightMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen K�nigbewegungen
		static MoveList getValidKingMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen Turmbewegungen
		static MoveList getValidRookMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle legalen L�uferbewegungen
		static MoveList getValidBishopMoves(Position& position, INT32 x, INT32 y, short player);
		/// Suche alle Linien ab f�r L�ufer, T�rme und Dame
		static MoveList continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir);

		/// Enemy-String nach Spielertyp ausgeben
		static std::string getEnemyPiecesString(short player);
	};
}