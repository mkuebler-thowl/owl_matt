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
		std::vector<Move> getValidMoves(const Position& position, short player, bool sort = false);

		/// <summary>
		/// Zug auf eine Position anwenden. Hinweis: Diese Funktion �berpr�ft nicht, ob der Zug auf die Position angwendet werden darf. In der Zugfindung sp�ter wichtig, in der �berpr�fungphase irrelevant.
		/// </summary>
		/// <param name="position:">Aktuelle Spielposition bzw. Ausgangsstellung</param>
		/// <param name="move">Der Zug, der ausgef�hrt werden soll</param>
		/// <returns>die neue Spielposition bzw. Ausgangsstellung</returns>
		const Position& applyMove(const Position& position, const Move& move);

		/// <summary>
		/// �berpr�fung, ob der K�nig durch den n�chsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der K�nig im Schach stehen w�rde</returns>
		bool isKinginCheckAfterMove(const Position& position, short player, const Move& move);

		/// <summary>
		/// Neu Spielposition zur Repition Map hinzuf�gen. Wird f�r die dreimalige Stellungswiederholung verwendet.
		/// </summary>
		/// <param name="position:">Die jeweilige Stellung</param>
		void addAppliedMoveToRepitionMap(Position& position);
	private:
		/// <summary>
		/// Funktion zur �berpr�fung, ob die aktuelle Stellung den jeweiligen K�nig bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der K�nig des aktuellen Spielers im Schach?</returns>
		bool isKingInCheck(const Position& position, short player);

		/// �berpr�fe alle diagonalen Felder vom K�nig ausgesehen
		bool checkKingDiagonal(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle horizontalen und vertikalen Felder vom K�nig ausgesehen
		bool checkKingAxis(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Springerbedrohungen vom K�nig ausgesehen
		bool checkKingKnights(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Bauerbedrohungen vom K�nig ausgesehen
		bool checkKingPawns(int king_x, int king_y, const Position& position, short player);

		/// Suche alle legalen Bauerbewegungen
		std::vector<Move> getValidPawnMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Springerbewegungen
		std::vector<Move> getValidKnightMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen K�nigbewegungen
		std::vector<Move> getValidKingMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Turmbewegungen
		std::vector<Move> getValidRookMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen L�uferbewegungen
		std::vector<Move> getValidBishopMoves(const Position& position, int x, int y, short player);
		/// Suche alle Linien ab f�r L�ufer, T�rme und Dame
		std::vector<Move> continueValidMovesOnLine(const Position& position, int x, int y, const std::string& enemies_string, int xDir, int yDir);

		/// �berpr�fe ob der Index die Schachgrenzen an x,y �berschreitet
		static bool isInsideChessboard(int x, int y);

		/// <summary>
		/// Stellungswiederholung-Dictionary
		/// string: Die jeweilige fen-string
		/// int: Die Anzahl der sich wiederholenden Stellung
		/// </summary>
		std::map<std::string, int> m_repitionMap;
	};
}