#pragma once

#include <string>
#include <array>
#include <stack>

#include "defines.hpp"
#include "Move.hpp"

namespace owl
{
	/**
	 * Mögliche Spielzustände einer Stellung.
	 */
	enum class GameState
	{
		Active, // Aktiv
		PlayerWhiteWins, // Weiß hat gewonnen
		PlayerBlackWins, // Schwarz hat gewonnen
		Remis // Unentschieden
	};

	/**
	 * Mögliche Spielphasen einer Stellung.
	 */
	enum class GamePhase
	{
		Opening, // Eröffnung
		Mid, // Mittelspiel
		End // Endspiel
	};

	/**
	 * Datenstruktur für eine beliebige Schachposition. 
	 * Beinhaltet ein 8x8 CHAR-Array, um eine jeweilige Stellung zu speichern.
	 */
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();
		/**
		 * Schachposition initialisieren. Wird bei Auslesen von FEN-Strings benötigt.
		 * 
		 * \param data Daten-Array für das Schachfeld
		 * \param player Der Spieler der aktuell dran ist
		 * \param whiteCastlingShort Kann Weiß noch kurz rochieren?
		 * \param whiteCastlingLong Kann Weiß noch lang rochieren?
		 * \param blackCastlingShort Kann Schwarz noch kurz rochieren?
		 * \param blackCastlingLong Kann Schwarz noch lange rochieren?
		 * \param enPassant Wurde ein En passant begangen?
		 * \param enPassantPosition Übergangenes En passant Feld
		 * \param moveCount Anzahl der Halbzüge seitdem kein Bauer bewegt oder eine Figur geschlagen wurde
		 * \param moveNumber Aktuelle Zugnummer, die nachdem Schwarz dran war, inkrementiert wird
		 */
		Position(const BOARD_ARRAY& data, INT32 player,
			BOOL whiteCastlingShort, BOOL whiteCastlingLong, 
			BOOL blackCastlingShort, BOOL blackCastlingLong, 
			BOOL enPassant, PAIR<INT32, INT32> enPassantPosition,
			INT32 moveCount, INT32 moveNumber);
		/**
		 * Operatorüberladungs [index], so dass über Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		 * 
		 * \param index Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)
		 * \return Gibt das jeweilige Zeilenarray[index] zurück
		 */
		BOARD_LINE& operator[](INT32 index) const;
		/**
		 * Führe einen neuen Zug auf die Position aus.
		 * 
		 * \param move Der neue Zug
		 */
		VOID applyMove(const Move& move);
		/**
		 * Mache den letzten Zug rückgängig.
		 */
		VOID undoLastMove();
		/**
		 * Setzte die Korordinaten, wo das En-Passant begangen wurde.
		 * 
		 * \param x Spalte
		 * \param y Zeile
		 */
		VOID setEnPassant(INT32 x, INT32 y);
		/**
		 * En Passant zurücksetzen.
		 */
		VOID resetEnPassant();
		/** 
		 * En Passant auslesen
		 * 
		 * \return En Passant?
		 */
		BOOL isEnPassant() const;
		/**
		 * Gib die Koordinaten des En-Passant zurück.
		 * 
		 * \return Zweier-Tupel der x,y-Koordinaten
		 */
		const PAIR<INT32, INT32>& getEnPassant() const;
		/**
		 * Halbzug für 50-Züge-Regel hinzutragen
		 */
		VOID addPlyCount();
		/**
		 * Aktuelle Halbzüge für die 50-Züge-Regel auf 0 setzen.
		 */
		VOID resetPlyCount();
		/**
		 * Aktuelle Halbzüge für die 50-Züge-Regel abfragen
		 * 
		 * \return 
		 */
		INT32 getPlyCount() const;
		/**
		 * Aktuelle Zugnummer zurückgeben.
		 * 
		 * \return Zugnummer
		 */
		INT32 getMoveNumber() const;
		/**
		 * Wechsele den aktuellen Spieler.
		 * 
		 * \param undoMove Wird der Zug rückgängig gemacht? (für Dekrement der Zugnummer)
		 */
		VOID changePlayer(BOOL undoMove = false);
		/**
		 * Gebe den aktuellen Spieler zurück.
		 * 
		 * \return Aktueller Spieler
		 */
		INT32 getPlayer() const;
		/**
		 * Setze einen neuen Spielzustand.
		 * 
		 * \param state der Spielzustand der gesetzt werden soll
		 */
		VOID setGameState(GameState state) const;
		/**
		 * Den aktuellen Spielzustand auslesen.
		 *
		 * \return Aktueller Spielzustand
		 */
		GameState getGameState() const;

		/**
		 * Kann weiß noch kurz rochieren?
		 */
		BOOL getWhiteCastlingShort() const;
		/**
		 * Kann Weiß noch lang rochieren? 
		 */
		BOOL getWhiteCastlingLong() const;
		/**
		 * Kann Schwarz noch kurz rochieren?
		 */
		BOOL getBlackCastlingShort() const;
		/**
		 * Kann Schwarz noch lang rochieren?
		 */
		BOOL getBlackCastlingLong() const;
		/**
		 * Deaktiviere kurze Rochade für Weiß.
		 */
		VOID resetWhiteCastlingShort();
		/**
		 * Deaktiviere lange Rochade für Weiß.
		 */
		VOID resetWhiteCastlingLong();
		/**
		 * Deaktiviere kurze Rochade für Schwarz.
		 */
		VOID resetBlackCastlingShort();
		/**
		 * Deaktiviere lange Rochade für Schwarz.
		 */
		VOID resetBlackCastlingLong();
		/**
		 * Betrete die nächste Spielphase.
		 */
		VOID enterNextGamePhase() const;
		/**
		 * Aktuelle Spielphase abrufen.
		 * 
		 * \return Aktuelle Spielphase
		 */
		GamePhase getGamePhase() const;

		/**
		 * Print-Funktion zur Ausgabe der Stellung.
		 * Zum Beispiel:
		 * rnbqkbnr
		 * pppppppp
		 * 
		 * 
		 * 
		 * 
		 * PPPPPPPP
		 * RNBQKBNR
		 */
		VOID print() const; 

		/**
		 * Gebe die Königsposition zurück.
		 * 
		 * \param index Spielerfarbe (WHITE_INDEX, BLACK_INDEX)
		 * \return Königsposition als Paar der Koordinaten x,y
		 */
		PAIR<INT32, INT32> getKingPosition(INT32 index) const;

		/**
		 * Sind die Figuren identisch platziert mit einer anderen Stellung?
		 * 
		 * \param other
		 * \return 
		 */
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

		/**
		 * Zugdaten für den Zugstapel, um Züge rückgängig machen zu können.
		 * Alle relevanten Zuginformationen, die durch applyMove(), undoLastMove() notwendig sind werden hier zwischengespeichert.
		 */
		struct MoveData
		{
			Move move; // Der Zug
			CHAR piece; // Figur des Zugs
			CHAR capturedPiece; // Mögliche geschlagene Figur (Zur Wiederbelebung)
			BOOL enPassantFlag; // War der Zug zuvor ein En Passant
			PAIR<INT32, INT32> enPassantPos; // Position des vorherigen En Passants
			PAIR<BOOL, INT32> plyCountReset; // Halbzüge-Reset? und vorheriger Halbzug-Wert
			UCHAR movedFirstTimeFlag; // Bitflags: Haben sich Turm oder König das erste mal bewegt
			GamePhase lastGamePhase; // Zum setzten der alten Phase, die durch die Evaluierung verändert wurde
		};
		/**
		 * Gebe den MoveData-Stapel zurück.
		 */
		const std::stack<MoveData>& getMoveDataStack() const;
	private:
		mutable BOARD_ARRAY m_data; // Datenobjekt für eine Spielposition
		mutable std::stack<MoveData> m_moveDataStack; // MoveData-Stapel für Zuginformationen
		INT32 m_kingPosition[VEC][PLAYER_COUNT]; // Königsposition der Spieler Weiß und Schwarz
		PAIR<INT32, INT32> m_enPassantPosition; // En Passant Position
		INT32 m_plyCount; // Anzahl der Halbszüge zur Bestimmung der 50-Züge-Regel
		INT32 m_moveNumber; // Nummerierung der Folgezüge. Beginnt bei 1 und wird nachdem Schwarz dran war, um 1 erhöht.
		INT32 m_player; // Aktueller Spieler
		
		mutable GameState m_state; // Korrespondierender Spielzustand zur Position (Aktiv, Sieg für Weiß/Schwarz oder Remis)
		mutable GamePhase m_gamePhase; // Spielphase

		BOOL m_enPassant; // En Passant möglich?
		BOOL m_whiteCastlingShort; // Kann Weiß noch kurz rochieren?
		BOOL m_whiteCastlingLong; // Kann Weiß noch lang rochieren?
		BOOL m_blackCastlingShort; // Kann Schwarz kurz rochieren?
		BOOL m_blackCastlingLong; // Kann Schwarz noch lang rochieren?

		UCHAR m_movedFirstTime; // Bitflags für die erstmalige Bewegung von Turm oder Könige (Für die Rochade)
		constexpr static UCHAR HAS_NOT_MOVED_BIT			= 0;	 // Weder Turm noch König bewegt?
		constexpr static UCHAR HAS_WHITE_KING_MOVED_BIT		= BIT_1; // Weißer König bewegt?
		constexpr static UCHAR HAS_WHITE_ROOK_R_MOVED_BIT	= BIT_2; // Weißer Turm auf H1 bewegt?
		constexpr static UCHAR HAS_WHITE_ROOK_L_MOVED_BIT	= BIT_3; // Weißer Turm A1 links bewegt?
		constexpr static UCHAR HAS_BLACK_KING_MOVED_BIT		= BIT_4; // Schwarzer König bewegt?
		constexpr static UCHAR HAS_BLACK_ROOK_R_MOVED_BIT	= BIT_5; // Schwarzer Turm auf H8 bewegt?
		constexpr static UCHAR HAS_BLACK_ROOK_L_MOVED_BIT	= BIT_6; // Schwarzer Turm auf A8 bewegt?
		/**
		 * Überprüfe ob Turm oder König das erste mal bewegt wurden.
		 * Aktiviere anschließend Flag zu m_movesFirstTime;
		 * \param check Flag der überprüft werden soll
		 * \param movedFirstTime Zweiter Flag der überprüft werden soll
		 */
		VOID checkFirstMovement(UCHAR check, UCHAR& movedFirstTime);
		/**
		 * Berechne die Position des Königs.
		 */
		VOID calculateKingPositions();
		/**
		 * Setzte die neue Königsposition.
		 * 
		 * \param index Weißer/Schwarzer König (WHITE_INDEX, BLACK_INDEX)
		 * \param x Spalte
		 * \param y Reihe
		 */
		VOID setKingPosition(INT32 index, INT32 x, INT32 y);
#if DEBUG
	public:
		std::string M_FEN; // FEN-String der Position (Für Debug-Zwecke)
#endif
	};
}