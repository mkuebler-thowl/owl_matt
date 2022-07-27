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
	/// Datenstruktur f�r eine beliebige Schachposition. 
	/// Beinhaltet ein 8x8 char-Array, um eine jeweilige Stellung zu speichern.
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();

		Position(const std::string& fen);

		/// <summary>
		/// Operator�berladung [index], so dass �ber Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zur�ck.</returns>
		std::array<char, 8>& operator[](int index) const;

		/// <summary>
		/// En Passant setzen
		/// </summary>
		void setEnPassant(int x, int y);

		/// <summary>
		/// En Passant zur�cksetzen
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
		/// Halbz�ge auf 0 zur�cksetzen
		/// </summary>
		void resetMoveCount();

		/// <summary>
		/// aktuelle Halbz�ge abfragen
		/// </summary>
		/// <returns></returns>
		int getMoveCount() const;

		/// <summary>
		/// Wechsel den aktuellen Spieler
		/// </summary>
		void changePlayer();

		/// <summary>
		/// Gebe den aktuellen Spieler zur�ck
		/// </summary>
		/// <returns></returns>
		int getPlayer() const;

		/// <summary>
		/// GameState �ndern
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
		/// 8x8 char-Array als Datenobjekt f�r eine Spielposition
		/// </summary>
		mutable std::array<std::array<char, 8>, 8> m_data;

		bool m_enPassant;

		std::pair<int,int> m_enPassantPosition;

		/// <summary>
		/// Anzahl der Halbsz�ge zur Bestimmung der 50-Z�ge-Regel
		/// </summary>
		int m_movesCount;
		/// <summary>
		/// Aktueller Spieler
		/// </summary>
		int m_player;

		GameState m_state;
	};
}