#pragma once

#include <string>
#include <array>
#include <stack>

#include "defines.hpp"
#include "Move.hpp"

namespace owl
{
	/**
	 * M�gliche Spielzust�nde einer Stellung.
	 */
	enum class GameState
	{
		Active, // Aktiv
		PlayerWhiteWins, // Wei� hat gewonnen
		PlayerBlackWins, // Schwarz hat gewonnen
		Remis // Unentschieden
	};

	/**
	 * M�gliche Spielphasen einer Stellung.
	 */
	enum class GamePhase
	{
		Opening, // Er�ffnung
		Mid, // Mittelspiel
		End // Endspiel
	};

	/**
	 * Datenstruktur f�r eine beliebige Schachposition. 
	 * Beinhaltet ein 8x8 CHAR-Array, um eine jeweilige Stellung zu speichern.
	 */
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();
		/**
		 * Schachposition initialisieren. Wird bei Auslesen von FEN-Strings ben�tigt.
		 * 
		 * \param data Daten-Array f�r das Schachfeld
		 * \param player Der Spieler der aktuell dran ist
		 * \param whiteCastlingShort Kann Wei� noch kurz rochieren?
		 * \param whiteCastlingLong Kann Wei� noch lang rochieren?
		 * \param blackCastlingShort Kann Schwarz noch kurz rochieren?
		 * \param blackCastlingLong Kann Schwarz noch lange rochieren?
		 * \param enPassant Wurde ein En passant begangen?
		 * \param enPassantPosition �bergangenes En passant Feld
		 * \param moveCount Anzahl der Halbz�ge seitdem kein Bauer bewegt oder eine Figur geschlagen wurde
		 * \param moveNumber Aktuelle Zugnummer, die nachdem Schwarz dran war, inkrementiert wird
		 */
		Position(const BOARD_ARRAY& data, INT32 player,
			BOOL whiteCastlingShort, BOOL whiteCastlingLong, 
			BOOL blackCastlingShort, BOOL blackCastlingLong, 
			BOOL enPassant, PAIR<INT32, INT32> enPassantPosition,
			INT32 moveCount, INT32 moveNumber);
		/**
		 * Operator�berladungs [index], so dass �ber Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		 * 
		 * \param index Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)
		 * \return Gibt das jeweilige Zeilenarray[index] zur�ck
		 */
		BOARD_LINE& operator[](INT32 index) const;
		/**
		 * F�hre einen neuen Zug auf die Position aus.
		 * 
		 * \param move Der neue Zug
		 */
		VOID applyMove(const Move& move);
		/**
		 * Mache den letzten Zug r�ckg�ngig.
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
		 * En Passant zur�cksetzen.
		 */
		VOID resetEnPassant();
		/** 
		 * En Passant auslesen
		 * 
		 * \return En Passant?
		 */
		BOOL isEnPassant() const;
		/**
		 * Gib die Koordinaten des En-Passant zur�ck.
		 * 
		 * \return Zweier-Tupel der x,y-Koordinaten
		 */
		const PAIR<INT32, INT32>& getEnPassant() const;
		/**
		 * Halbzug f�r 50-Z�ge-Regel hinzutragen
		 */
		VOID addPlyCount();
		/**
		 * Aktuelle Halbz�ge f�r die 50-Z�ge-Regel auf 0 setzen.
		 */
		VOID resetPlyCount();
		/**
		 * Aktuelle Halbz�ge f�r die 50-Z�ge-Regel abfragen
		 * 
		 * \return 
		 */
		INT32 getPlyCount() const;
		/**
		 * Aktuelle Zugnummer zur�ckgeben.
		 * 
		 * \return Zugnummer
		 */
		INT32 getMoveNumber() const;
		/**
		 * Wechsele den aktuellen Spieler.
		 * 
		 * \param undoMove Wird der Zug r�ckg�ngig gemacht? (f�r Dekrement der Zugnummer)
		 */
		VOID changePlayer(BOOL undoMove = false);
		/**
		 * Gebe den aktuellen Spieler zur�ck.
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
		 * Kann wei� noch kurz rochieren?
		 */
		BOOL getWhiteCastlingShort() const;
		/**
		 * Kann Wei� noch lang rochieren? 
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
		 * Deaktiviere kurze Rochade f�r Wei�.
		 */
		VOID resetWhiteCastlingShort();
		/**
		 * Deaktiviere lange Rochade f�r Wei�.
		 */
		VOID resetWhiteCastlingLong();
		/**
		 * Deaktiviere kurze Rochade f�r Schwarz.
		 */
		VOID resetBlackCastlingShort();
		/**
		 * Deaktiviere lange Rochade f�r Schwarz.
		 */
		VOID resetBlackCastlingLong();
		/**
		 * Betrete die n�chste Spielphase.
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
		 * Gebe die K�nigsposition zur�ck.
		 * 
		 * \param index Spielerfarbe (WHITE_INDEX, BLACK_INDEX)
		 * \return K�nigsposition als Paar der Koordinaten x,y
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
		 * Zugdaten f�r den Zugstapel, um Z�ge r�ckg�ngig machen zu k�nnen.
		 * Alle relevanten Zuginformationen, die durch applyMove(), undoLastMove() notwendig sind werden hier zwischengespeichert.
		 */
		struct MoveData
		{
			Move move; // Der Zug
			CHAR piece; // Figur des Zugs
			CHAR capturedPiece; // M�gliche geschlagene Figur (Zur Wiederbelebung)
			BOOL enPassantFlag; // War der Zug zuvor ein En Passant
			PAIR<INT32, INT32> enPassantPos; // Position des vorherigen En Passants
			PAIR<BOOL, INT32> plyCountReset; // Halbz�ge-Reset? und vorheriger Halbzug-Wert
			UCHAR movedFirstTimeFlag; // Bitflags: Haben sich Turm oder K�nig das erste mal bewegt
			GamePhase lastGamePhase; // Zum setzten der alten Phase, die durch die Evaluierung ver�ndert wurde
		};
		/**
		 * Gebe den MoveData-Stapel zur�ck.
		 */
		const std::stack<MoveData>& getMoveDataStack() const;
	private:
		mutable BOARD_ARRAY m_data; // Datenobjekt f�r eine Spielposition
		mutable std::stack<MoveData> m_moveDataStack; // MoveData-Stapel f�r Zuginformationen
		INT32 m_kingPosition[VEC][PLAYER_COUNT]; // K�nigsposition der Spieler Wei� und Schwarz
		PAIR<INT32, INT32> m_enPassantPosition; // En Passant Position
		INT32 m_plyCount; // Anzahl der Halbsz�ge zur Bestimmung der 50-Z�ge-Regel
		INT32 m_moveNumber; // Nummerierung der Folgez�ge. Beginnt bei 1 und wird nachdem Schwarz dran war, um 1 erh�ht.
		INT32 m_player; // Aktueller Spieler
		
		mutable GameState m_state; // Korrespondierender Spielzustand zur Position (Aktiv, Sieg f�r Wei�/Schwarz oder Remis)
		mutable GamePhase m_gamePhase; // Spielphase

		BOOL m_enPassant; // En Passant m�glich?
		BOOL m_whiteCastlingShort; // Kann Wei� noch kurz rochieren?
		BOOL m_whiteCastlingLong; // Kann Wei� noch lang rochieren?
		BOOL m_blackCastlingShort; // Kann Schwarz kurz rochieren?
		BOOL m_blackCastlingLong; // Kann Schwarz noch lang rochieren?

		UCHAR m_movedFirstTime; // Bitflags f�r die erstmalige Bewegung von Turm oder K�nige (F�r die Rochade)
		constexpr static UCHAR HAS_NOT_MOVED_BIT			= 0;	 // Weder Turm noch K�nig bewegt?
		constexpr static UCHAR HAS_WHITE_KING_MOVED_BIT		= BIT_1; // Wei�er K�nig bewegt?
		constexpr static UCHAR HAS_WHITE_ROOK_R_MOVED_BIT	= BIT_2; // Wei�er Turm auf H1 bewegt?
		constexpr static UCHAR HAS_WHITE_ROOK_L_MOVED_BIT	= BIT_3; // Wei�er Turm A1 links bewegt?
		constexpr static UCHAR HAS_BLACK_KING_MOVED_BIT		= BIT_4; // Schwarzer K�nig bewegt?
		constexpr static UCHAR HAS_BLACK_ROOK_R_MOVED_BIT	= BIT_5; // Schwarzer Turm auf H8 bewegt?
		constexpr static UCHAR HAS_BLACK_ROOK_L_MOVED_BIT	= BIT_6; // Schwarzer Turm auf A8 bewegt?
		/**
		 * �berpr�fe ob Turm oder K�nig das erste mal bewegt wurden.
		 * Aktiviere anschlie�end Flag zu m_movesFirstTime;
		 * \param check Flag der �berpr�ft werden soll
		 * \param movedFirstTime Zweiter Flag der �berpr�ft werden soll
		 */
		VOID checkFirstMovement(UCHAR check, UCHAR& movedFirstTime);
		/**
		 * Berechne die Position des K�nigs.
		 */
		VOID calculateKingPositions();
		/**
		 * Setzte die neue K�nigsposition.
		 * 
		 * \param index Wei�er/Schwarzer K�nig (WHITE_INDEX, BLACK_INDEX)
		 * \param x Spalte
		 * \param y Reihe
		 */
		VOID setKingPosition(INT32 index, INT32 x, INT32 y);
#if DEBUG
	public:
		std::string M_FEN; // FEN-String der Position (F�r Debug-Zwecke)
#endif
	};
}