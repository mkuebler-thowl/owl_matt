#pragma once
#include <string>
#include <vector>
#include <map>

#include "Move.hpp"
#include "Position.hpp"

namespace matt
{
	/// ChessValidation bietet als Hilfsklasse f�r Funktionen zur �berpr�fung und Ermittlung legaler Z�ge.
	class ChessValidation
	{
	public:
		/// <summary>
		/// Funktion zur Ermittlung g�ltiger Z�ge "Moves".
		/// </summary>
		/// <param name="position:">Aktuelle Spielstellung bzw. Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler, der am Zug ist.</param>
		/// <param name="sort:">Bei Aktivierung wird die Liste zus�tzlich nach Nutzen sortiert. (Iterative Deeping)</param>
		/// <returns>Liste der m�glichen bzw. g�ltigen Z�ge</returns>
		static std::vector<Move> getValidMoves(const Position& position, short player, bool sort = false);

		/// <summary>
		/// Zug auf eine Position anwenden. Hinweis: Diese Funktion �berpr�ft nicht, ob der Zug auf die Position angwendet werden darf. In der Zugfindung sp�ter wichtig, in der �berpr�fungphase irrelevant.
		/// </summary>
		/// <param name="position:">Aktuelle Spielposition bzw. Ausgangsstellung</param>
		/// <param name="move">Der Zug, der ausgef�hrt werden soll</param>
		/// <returns>die neue Spielposition bzw. Ausgangsstellung</returns>
		static const Position& applyMove(const Position& position, const Move& move);

		/// <summary>
		/// �berpr�fung, ob der K�nig durch den n�chsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der K�nig im Schach stehen w�rde</returns>
		static bool isKinginCheckAfterMove(const Position& position, short player, const Move& move);


	private:
		/// <summary>
		/// Funktion zur �berpr�fung, ob die aktuelle Stellung den jeweiligen K�nig bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der K�nig des aktuellen Spielers im Schach?</returns>
		static bool isKingInCheck(const Position& position, short player);
		/// <summary>
		/// Ist die Platzierung x,y im Schach bzw. wird diese Position bedroht? 
		/// </summary>
		/// <param name="position"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="player"></param>
		/// <returns></returns>
		static bool isPlaceInCheck(const Position& position, int x, int y, short player);

		/// �berpr�fe alle diagonalen Felder vom K�nig ausgesehen
		static bool checkKingDiagonal(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle horizontalen und vertikalen Felder vom K�nig ausgesehen
		static bool checkKingAxis(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Springerbedrohungen vom K�nig ausgesehen
		static bool checkKingKnights(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Bauerbedrohungen vom K�nig ausgesehen
		static bool checkKingPawns(int king_x, int king_y, const Position& position, short player);

		/// Schachmatt �berpr�fen
		static void evaluateCheckmate(const Position& position, short player, bool noValidMoves);

		/// Suche alle legalen Bauerbewegungen
		static std::vector<Move> getValidPawnMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Springerbewegungen
		static std::vector<Move> getValidKnightMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen K�nigbewegungen
		static std::vector<Move> getValidKingMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Turmbewegungen
		static std::vector<Move> getValidRookMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen L�uferbewegungen
		static std::vector<Move> getValidBishopMoves(const Position& position, int x, int y, short player);
		/// Suche alle Linien ab f�r L�ufer, T�rme und Dame
		static std::vector<Move> continueValidMovesOnLine(const Position& position, int x, int y, const std::string& enemies_string, int xDir, int yDir);

		/// �berpr�fe ob die Stelle x,y die Spielfeldgrenzen �berschreitet
		static bool isInsideChessboard(int x, int y);
	};
}