#pragma once

#include <string>
#include <array>
#include <stack>

#include "defines.hpp"
#include "Move.hpp"

namespace owl
{
	/// Enumeration für mögliche Spielzustände einer Schachposition.
	enum class GameState
	{
		Active,
		PlayerWhiteWins,
		PlayerBlackWins,
		Remis
	};

	enum class GamePhase
	{
		Opening,
		Mid,
		End
	};


 // Board

	/// Datenstruktur für eine beliebige Schachposition. 
	/// Beinhaltet ein 8x8 CHAR-Array, um eine jeweilige Stellung zu speichern.
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
		Position(const BOARD_ARRAY& data, INT16 player,
			BOOL whiteCastlingShort, BOOL whiteCastlingLong, 
			BOOL blackCastlingShort, BOOL blackCastlingLong, 
			BOOL enPassant, PAIR<UINT16, UINT16> enPassantPosition,
			UINT16 moveCount, UINT16 moveNumber);

		/// <summary>
		/// Operatorüberladungs [index], so dass über Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zurück.</returns>
		BOARD_LINE& operator[](INT32 index) const;

		VOID applyMove(const Move& move);
		VOID undoLastMove();

		/// En Passant setzen
		VOID setEnPassant(INT32 x, INT32 y);
		/// En Passant zurücksetzen
		VOID resetEnPassant();
		/// En Passant auslesen
		BOOL isEnPassant() const;

		/// En Passant-Position auslesen
		const PAIR<INT32, INT32>& getEnPassant() const;

		/// Halbzug hinzutragen
		VOID addPlyCount();
		/// Halbzüge auf 0 zurücksetzen
		VOID resetPlyCount();
		/// aktuelle Halbzüge abfragen
		UINT16 getPlyCount() const;
		/// aktuelle Zugnummer abfragen
		UINT16 getMoveNumber() const;

		/// Wechsel den aktuellen Spieler
		VOID changePlayer(BOOL undoMove = false);
		/// Gibt den aktuellen Spieler zurück (Der dran ist)
		INT16 getPlayer() const;

		/// GameState ändern
		VOID setGameState(GameState state) const;
		/// GameState abrufen
		GameState getGameState() const;

		/// Kann Weiß noch kurz rochieren?
		BOOL getWhiteCastlingShort() const;
		/// Kann Weiß noch lang rochieren?
		BOOL getWhiteCastlingLong() const;
		/// Kann Schwarz noch kurz rochieren?
		BOOL getBlackCastlingShort() const;
		/// Kann Schwarz noch lang rochieren?
		BOOL getBlackCastlingLong() const;
		/// Für Weiß: Kurz-Rochade deaktivieren
		VOID resetWhiteCastlingShort();
		/// Für Weiß: Lang-Rochade deaktivieren
		VOID resetWhiteCastlingLong();
		/// Für Schwarz: Kurz-Rochade deaktivieren
		VOID resetBlackCastlingShort();
		/// Für Schwarz: Lang-Rochade deaktivieren
		VOID resetBlackCastlingLong();

		/// Spielphase aktualisieren
		VOID enterNextGamePhase() const;

		/// Spielphase abrufen
		GamePhase getGamePhase() const;

		static VOID printPosition(const Position& position); 

		constexpr BOOL operator==(const Position& other) const
		{
			for (auto y = FIRST_ROW_INDEX; y < ROWS; y++)
			{
				for (auto x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
				{
					if (m_data[y][x] != other[y][x]) return false;
				}
			}
			return true;
		}

		struct MoveData
		{
			Move move; // Der Zug
			CHAR piece; // Figur des Zugs
			CHAR capturedPiece; // Mögliche geschlagene Figur (Zur Wiederbelebung)
			BOOL enPassantFlag; // War der Zug zuvor ein En Passant
			PAIR<UINT16, UINT16> enPassantPos; // Position des vorherigen En Passants
			PAIR<BOOL, UINT16> plyCountReset; // Halbzüge-Reset? und vorheriger Halbzug-Wert
			UCHAR movedFirstTimeFlag; // Bitflags: Haben sich Turm oder König das erste mal bewegt
			GamePhase lastGamePhase; // Zum setzten der alten Phase, die durch die Evaluierung verändert wurde
		};
	private:

		/// Datenobjekt für eine Spielposition
		mutable BOARD_ARRAY m_data;
		std::stack<MoveData> m_moveDataStack;

		/// En Passant möglich?
		BOOL m_enPassant;

		/// En Passant Position
		PAIR<UINT16, UINT16> m_enPassantPosition;
		/// Anzahl der Halbszüge zur Bestimmung der 50-Züge-Regel
		UINT16 m_plyCount;
		/// Nummerierung der Folgezüge. Beginnt bei 1 und wird nachdem Schwarz dran war, um eins erhöht.
		UINT16 m_moveNumber;

		/// Aktueller Spieler
		INT16 m_player;
		/// Kann Weiß noch kurz rochieren?
		BOOL m_whiteCastlingShort;
		/// Kann Weiß noch lang rochieren?
		BOOL m_whiteCastlingLong;
		/// Kann Schwarz noch kurz rochieren?
		BOOL m_blackCastlingShort;
		/// Kann Schwarz noch lang rochieren?
		BOOL m_blackCastlingLong;

		UCHAR m_movedFirstTime;

		constexpr static UCHAR HAS_NOT_MOVED_BIT = 0;
		constexpr static UCHAR HAS_WHITE_KING_MOVED_BIT = BIT_1;
		constexpr static UCHAR HAS_WHITE_ROOK_R_MOVED_BIT = BIT_2;
		constexpr static UCHAR HAS_WHITE_ROOK_L_MOVED_BIT = BIT_3;
		constexpr static UCHAR HAS_BLACK_KING_MOVED_BIT = BIT_4;
		constexpr static UCHAR HAS_BLACK_ROOK_R_MOVED_BIT = BIT_5;
		constexpr static UCHAR HAS_BLACK_ROOK_L_MOVED_BIT = BIT_6;

		VOID checkFirstMovement(UCHAR check, UCHAR& movedFirstTime);

		/// Korrespondierender Spielzustand zur Position (Aktiv, Sieg für Weiß/Schwarz oder Remis)
		mutable GameState m_state;

		/// Spielphase
		mutable GamePhase m_gamePhase;
	};
}