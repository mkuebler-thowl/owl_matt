#pragma once

#include <string>
#include <array>

namespace matt
{
	enum GameState
	{
		Active,
		PlayerWhiteWins,
		PlayerBlackWins,
		Remis
	};
	/// Datenstruktur für eine beliebige Schachposition. 
	/// Beinhaltet ein 8x8 char-Array, um eine jeweilige Stellung zu speichern.
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();

		Position(const std::string& fen);

		/// <summary>
		/// Operatorüberladung [index], so dass über Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zurück.</returns>
		std::array<char, 8>& operator[](int index) const;

		/// <summary>
		/// En Passant setzen
		/// </summary>
		void setEnPassant(int x, int y);

		/// <summary>
		/// En Passant zurücksetzen
		/// </summary>
		void resetEnPassant();

		/// <summary>
		/// En Passant auslesen
		/// </summary>
		bool isEnPassant() const;

		/// <summary>
		/// En Passant-Position auslesen
		/// </summary>
		/// <returns></returns>
		const std::pair<int, int>& getEnPassant() const;

		/// <summary>
		/// Halbzug hinzutragen
		/// </summary>
		void addMoveCount();

		/// <summary>
		/// Halbzüge auf 0 zurücksetzen
		/// </summary>
		void resetMoveCount();

		/// <summary>
		/// aktuelle Halbzüge abfragen
		/// </summary>
		/// <returns></returns>
		int getMoveCount() const;

		/// <summary>
		/// Wechsel den aktuellen Spieler
		/// </summary>
		void changePlayer();

		/// <summary>
		/// Gebe den aktuellen Spieler zurück
		/// </summary>
		/// <returns></returns>
		int getPlayer() const;

		/// <summary>
		/// GameState ändern
		/// </summary>
		/// <param name="state">neuer State</param>
		void setGameState(GameState state);

		/// <summary>
		/// GameState abrufen
		/// </summary>
		/// <returns></returns>
		GameState getGameState() const;
	private:
		/// <summary>
		/// 8x8 char-Array als Datenobjekt für eine Spielposition
		/// </summary>
		mutable std::array<std::array<char, 8>, 8> m_data;

		bool m_enPassant;

		std::pair<int,int> m_enPassantPosition;

		/// <summary>
		/// Anzahl der Halbszüge zur Bestimmung der 50-Züge-Regel
		/// </summary>
		int m_movesCount;
		/// <summary>
		/// Aktueller Spieler
		/// </summary>
		int m_player;

		GameState m_state;
	};
}