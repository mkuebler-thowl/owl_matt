#pragma once

namespace matt
{
	/// Datenstruktur zur Information eines Spielzugs, der auf eine Spielstellung ausgeführt werden kann.
	struct Move
	{
		/// Startfeld (Spalte)
		unsigned short startX;
		/// Startfeld (Reihe)
		unsigned short startY;

		/// Zielfeld (Spalte)
		unsigned short targetX;
		/// Zielfeld (Reihe)
		unsigned short targetY;

		/// Figure geschlagen
		bool capture;
		/// Bauernumwandlung
		char promotion;
	};
}