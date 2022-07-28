#pragma once

#include <string>
#include <array>

namespace matt
{
	/// Enumeration für mögliche Spielzustände einer Schachposition.
	enum class GameState
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

		/// <summary>
		/// Schachposition initialisieren. Wird bei Auslesen von FEN-Strings benötigt.
		/// </summary>
		/// <param name="data:">Daten-Array für das Schachfeld</param>
		/// <param name="player:">Der Spieler der aktuell dran ist</param>
		/// <param name="whiteCastlingShort:">Kann Weiß noch kurz rochieren?</param>
		/// <param name="whiteCastlingLong:">Kann Weiß noch lang rochieren?</param>
		/// <param name="blackCastlingShort:">Kann Schwarz noch kurz rochieren?</param>
		/// <param name="blackCastlingLong:">Kann Schwarz noch lange rochieren?</param>
		/// <param name="enPassant:">Wurde ein En passant begangen?</param>
		/// <param name="enPassantPosition:">Übergangenes En passant Feld</param>
		/// <param name="moveCount:">Anzahl der Halbzüge seitdem kein Bauer bewegt oder eine Figur geschlagen wurde</param>
		/// <param name="moveNumber:">Aktuelle Zugnummer, die nachdem Schwarz dran war, inkrementiert wird</param>
		Position(const std::array<std::array<char, 8>, 8> data, short player, 
			bool whiteCastlingShort, bool whiteCastlingLong, 
			bool blackCastlingShort, bool blackCastlingLong, 
			bool enPassant, std::pair<int, int> enPassantPosition, 
			int moveCount, int moveNumber);

		/// <summary>
		/// Operatorüberladung [index], so dass über Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zurück.</returns>
		std::array<char, 8>& operator[](int index) const;

		/// En Passant setzen
		void setEnPassant(int x, int y);
		/// En Passant zurücksetzen
		void resetEnPassant();
		/// En Passant auslesen
		bool isEnPassant() const;

		/// En Passant-Position auslesen
		const std::pair<int, int>& getEnPassant() const;

		/// Halbzug hinzutragen
		void addMoveCount();
		/// Halbzüge auf 0 zurücksetzen
		void resetMoveCount();
		/// aktuelle Halbzüge abfragen
		int getMoveCount() const;
		/// aktuelle Zugnummer abfragen
		int getMoveNumber() const;

		/// Wechsel den aktuellen Spieler
		void changePlayer();
		/// Gibt den aktuellen Spieler zurück (Der dran ist)
		int getPlayer() const;

		/// GameState ändern
		void setGameState(GameState state) const;
		/// GameState abrufen
		GameState getGameState() const;

		/// Kann Weiß noch kurz rochieren?
		bool getWhiteCastlingShort() const;
		/// Kann Weiß noch lang rochieren?
		bool getWhiteCastlingLong() const;
		/// Kann Schwarz noch kurz rochieren?
		bool getBlackCastlingShort() const;
		/// Kann Schwarz noch lang rochieren?
		bool getBlackCastlingLong() const;
		/// Für Weiß: Kurz-Rochade deaktivieren
		void resetWhiteCastlingShort();
		/// Für Weiß: Lang-Rochade deaktivieren
		void resetWhiteCastlingLong();
		/// Für Schwarz: Kurz-Rochade deaktivieren
		void resetBlackCastlingShort();
		/// Für Schwarz: Lang-Rochade deaktivieren
		void resetBlackCastlingLong();
	private:
		/// 8x8 char-Array als Datenobjekt für eine Spielposition
		mutable std::array<std::array<char, 8>, 8> m_data;
		/// En Passant möglich?
		bool m_enPassant;
		/// En Passant Position
		std::pair<int,int> m_enPassantPosition;
		/// Anzahl der Halbszüge zur Bestimmung der 50-Züge-Regel
		int m_movesCount;
		/// Nummerierung der Folgezüge. Beginnt bei 1 und wird nachdem Schwarz dran war, um eins erhöht.
		int m_moveNumber;

		/// Aktueller Spieler
		int m_player;
		/// Kann Weiß noch kurz rochieren?
		bool m_whiteCastlingShort;
		/// Kann Weiß noch lang rochieren?
		bool m_whiteCastlingLong;
		/// Kann Schwarz noch kurz rochieren?
		bool m_blackCastlingShort;
		/// Kann Schwarz noch lang rochieren?
		bool m_blackCastlingLong;

		/// Korrespondierender Spielzustand zur Position (Aktiv, Sieg für Weiß/Schwarz oder Remis)
		mutable GameState m_state;
	};
}