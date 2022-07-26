#pragma once
#include <vector>
#include "Move.hpp"
#include "Position.hpp"

namespace matt
{
	/// ChessValidation bietet als Hilfsklasse statische Funktionen zur �berpr�fung und Ermittlung legaler Z�ge.
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
		/// Funktion zur �berpr�fung, ob die aktuelle Stellung den jeweiligen K�nig bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der K�nig des aktuellen Spielers im Schach?</returns>
		static bool isKingInCheck(const Position& position, short player);
	private:
		/// �berpr�fe alle diagonalen Felder vom K�nig ausgesehen
		static bool checkKingDiagonal(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle horizontalen und vertikalen Felder vom K�nig ausgesehen
		static bool checkKingAxis(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Springerbedrohungen vom K�nig ausgesehen
		static bool checkKingKnights(int king_x, int king_y, const Position& position, short player);
		/// �berpr�fe alle m�glichen Bauerbedrohungen vom K�nig ausgesehen
		static bool checkKingPawns(int king_x, int king_y, const Position& position, short player);
	};
}