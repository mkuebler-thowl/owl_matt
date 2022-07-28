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
		static std::vector<Move> getValidMoves(const Position& position, short player, bool sort = false);

		/// <summary>
		/// Zug auf eine Position anwenden. Hinweis: Diese Funktion überprüft nicht, ob der Zug auf die Position angwendet werden darf. In der Zugfindung später wichtig, in der Überprüfungphase irrelevant.
		/// </summary>
		/// <param name="position:">Aktuelle Spielposition bzw. Ausgangsstellung</param>
		/// <param name="move">Der Zug, der ausgeführt werden soll</param>
		/// <returns>die neue Spielposition bzw. Ausgangsstellung</returns>
		static const Position& applyMove(const Position& position, const Move& move);

		/// <summary>
		/// Überprüfung, ob der König durch den nächsten Zug im Schach steht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Aktueller Spieler</param>
		/// <param name="move:">Angewendeter Zug</param>
		/// <returns>Ob der König im Schach stehen würde</returns>
		static bool isKinginCheckAfterMove(const Position& position, short player, const Move& move);


	private:
		/// <summary>
		/// Funktion zur Überprüfung, ob die aktuelle Stellung den jeweiligen König bedroht.
		/// </summary>
		/// <param name="position:">Ausgangsstellung</param>
		/// <param name="player:">Der aktuelle Spieler, der am Zug ist</param>
		/// <returns>Steht der König des aktuellen Spielers im Schach?</returns>
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

		/// Überprüfe alle diagonalen Felder vom König ausgesehen
		static bool checkKingDiagonal(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle horizontalen und vertikalen Felder vom König ausgesehen
		static bool checkKingAxis(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle möglichen Springerbedrohungen vom König ausgesehen
		static bool checkKingKnights(int king_x, int king_y, const Position& position, short player);
		/// Überprüfe alle möglichen Bauerbedrohungen vom König ausgesehen
		static bool checkKingPawns(int king_x, int king_y, const Position& position, short player);

		/// Schachmatt überprüfen
		static void evaluateCheckmate(const Position& position, short player, bool noValidMoves);

		/// Suche alle legalen Bauerbewegungen
		static std::vector<Move> getValidPawnMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Springerbewegungen
		static std::vector<Move> getValidKnightMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Königbewegungen
		static std::vector<Move> getValidKingMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Turmbewegungen
		static std::vector<Move> getValidRookMoves(const Position& position, int x, int y, short player);
		/// Suche alle legalen Läuferbewegungen
		static std::vector<Move> getValidBishopMoves(const Position& position, int x, int y, short player);
		/// Suche alle Linien ab für Läufer, Türme und Dame
		static std::vector<Move> continueValidMovesOnLine(const Position& position, int x, int y, const std::string& enemies_string, int xDir, int yDir);

		/// Überprüfe ob die Stelle x,y die Spielfeldgrenzen überschreitet
		static bool isInsideChessboard(int x, int y);
	};
}