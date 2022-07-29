#pragma once

#include <vector>

#include "Position.hpp"
#include "Move.hpp"
#include "MinMaxResult.hpp"
#include "ChessValidation.hpp"

namespace matt
{
	// Feature-Parameter
	constexpr unsigned char FT_NULL			= 0;
	constexpr unsigned char FT_ALPHA_BETA	= (1 << 0);
	constexpr unsigned char FT_SORT			= (1 << 1);
	constexpr unsigned char FT_NESTED		= (1 << 2);
	constexpr unsigned char FT_KILLER		= (1 << 3);
	// Optionale-Paramater (nicht implementiert):
	constexpr unsigned char FT_HISTORY		= (1 << 4);
	constexpr unsigned char FT_PVS			= (1 << 5);

	// Spieler
	constexpr short PLAYER_WHITE = 1;
	constexpr short PLAYER_BLACK = -1;

	// Maximalwert
	constexpr float INF = 999.0f;

	/// Schach-Engine-Klasse. Beinhaltet die Logik einer Schach-Engine, wie die Zugfindung und Bewertung eines Zugs usw.
	class ChessEngine
	{
	public:
		explicit ChessEngine();
		virtual ~ChessEngine();

		// TODO: Threshold f�r Random irgendwie berechnen/festlegen
		/// <summary>
		/// Funktion f�r die Zugfindung aus einer Spielposition heraus
		/// </summary>
		/// <param name="position:">Ausgangsstellung der Schachposition bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. n�chstes am Zug ist</param>
		/// <param name="depth:">Die maximale Suchtiefe d</param>
		/// <param name="parameterFlags:">F�r Min-Max-Erweiterungen k�nnen Feature-Parameter �ber Bitflags aktiviert werden</param>
		/// <param name="random:">Bei Aktivierung wird aus einer Menge bestm�glicher Z�ge (+-Threshold) ein Zug zuf�llig gew�hlt</param>
		/// <returns>Der bestm�gliche gefundene Zug innerhalb der Suchtiefe</returns>
		Move searchMove(const Position& position, short player, unsigned short depth, unsigned char parameter_flags, bool random = false);
	private:
		// TODO: Vielleicht auslagen (um Klasse kleiner zu halten) Klassename: ChessEvaluation
		/// <summary>
		/// Bewertungsfunktion
		/// </summary>
		/// <param name="position:">Ausgangsstellung bzw. Spielposition, welche bewertet werden soll.</param>
		/// <param name="simple:">Wenn aktiv wird nur die einfache Materialbalance berechnet (Bei Sortierung m�glicherweise ausreichend)"</param>
		/// <returns>Nutzwert</returns>
		float evaluate(const Position& position, bool simple = false);

		/// <summary>
		/// Klassischer Min-Max-Algorithmus zur Zugfindung
		/// </summary>
		/// <param name="position:">Ausgangsstellung bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. n�chstes am Zug ist</param>
		/// <param name="depth:">maximale Suchtiefe d</param>
		/// <param name="sort:">Z�ge nach Nutzen sortieren?</param>
		/// <returns>Nutzwert</returns>
		MinMaxResult minMax(const Position& position, short player, unsigned short depth, bool sort = false);

		/// <summary>
		/// Alpha-Beta-Suche (Erweiterung des Min-Max)
		/// </summary>
		/// <param name="position">Ausgangsstellung bzw. Spielposition</param>
		/// <param name="player:">Spieler, der als erstes bzw. n�chstes am Zug ist</param>
		/// <param name="depth:">maximale Suchtiefe d</param>
		/// <param name="alpha:">Alpha-Wert</param>
		/// <param name="beta:">Beta-Wert</param>
		/// <param name="sort:">Z�ge nach Nutzen sortieren?</param>
		/// <returns></returns>
		float alphaBeta(const Position& position, short player, unsigned short depth, float alpha, float beta, bool sort = false);

	};
}