#pragma once
#include <string>
#include <vector>
#include <map>

#include "Move.hpp"
#include "Position.hpp"

namespace matt
{
	/// ChessValidation bietet als Hilfsklasse für Funktionen zur Überprüfung und Ermittlung legaler Züge.
	class ChessValidation
	{
	public:
		/// <summary>
		/// Funktion zur Ermittlung gültiger Züge "Moves".
		/// </summary>
		/// <param name="position:">Aktuelle Spielstellung bzw. Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler, der am Zug ist.</param>
		/// <param name="sort:">Bei Aktivierung wird die Liste zusätzlich nach Nutzen sortiert. (Iterative Deeping)</param>
		/// <returns>Liste der möglichen bzw. gültigen Züge</returns>
		std::vector<Move> getValidMoves(const Position& position, short player, bool sort = false);

		/// <summary>
		/// Zug auf eine Position anwenden. Hinweis: Diese Funktion überprüft nicht, ob der Zug auf die Position angwendet werden darf. In der Zugfindung später wichtig, in der Überprüfungphase irrelevant.
		/// </summary>
		/// <param name="position:">Aktuelle Spielposition bzw. Ausgangsstellung</param>
		/// <param name="move">Der Zug, der ausgeführt werden soll</param>
		/// <returns>die neue Spielposition bzw. Ausgangsstellung</returns>
		const Position& applyMove(const Position& position, const Move& move);

		/// <summary>
		/// Überprüfung, ob der König durch den nächsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der König im Schach stehen würde</returns>
		bool isKinginCheckAfterMove(const Position& position, short player, const Move& move);

		/// <summary>
		/// Neu Spielposition zur Repition Map hinzufügen. Wird für die dreimalige Stellungswiederholung verwendet.
		/// </summary>
		/// <param name="position:">Die jeweilige Stellung</param>
		void addAppliedMoveToRepitionMap(Position& position);
	private:
		/// <summary>
		/// Funktion zur Überprüfung, ob die aktuelle Stellung den jeweiligen König bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der König des aktuellen Spielers im Schach?</returns>
		bool isKingInCheck(const Position& position, short player);

		/// Überprüfe alle diagonalen Felder vom König ausgesehen
		bool checkKingDiagonal(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle horizontalen und vertikalen Felder vom König ausgesehen
		bool checkKingAxis(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle möglichen Springerbedrohungen vom König ausgesehen
		bool checkKingKnights(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle möglichen Bauerbedrohungen vom König ausgesehen
		bool checkKingPawns(int king_x, int king_y, const Position& position, short player);

		/// Suche alle legalen Bauerbewegungen
		std::vector<Move> getValidPawnMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Springerbewegungen
		std::vector<Move> getValidKnightMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Königbewegungen
		std::vector<Move> getValidKingMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Turmbewegungen
		std::vector<Move> getValidRookMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Läuferbewegungen
		std::vector<Move> getValidBishopMoves(const Position& position, int x, int y, short player);
		/// Suche alle Linien ab für Läufer, Türme und Dame
		std::vector<Move> continueValidMovesOnLine(const Position& position, int x, int y, const std::string& enemies_string, int xDir, int yDir);

		/// Überprüfe ob der Index die Schachgrenzen an x,y überschreitet
		static bool isInsideChessboard(int x, int y);

		/// <summary>
		/// Stellungswiederholung-Dictionary
		/// string: Die jeweilige fen-string
		/// int: Die Anzahl der sich wiederholenden Stellung
		/// </summary>
		std::map<std::string, int> m_repitionMap;
	};
}