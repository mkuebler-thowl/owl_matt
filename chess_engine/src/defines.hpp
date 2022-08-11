#pragma once

#include <array>
#include <vector>

namespace owl
{
	// Klassen deklarieren (F�r using ben�tigt)
	class Position;
	class ChessEngine;
	class ChessEvaluation;
	class ChessValidation;
	class RepitionMap;
	class FenParser;

	struct Move;
	struct MinMaxResult;

	// Typdefinitionen
	typedef short INT16;
	typedef unsigned short UINT16;
	typedef int INT32;
	typedef unsigned int UINT32;
	typedef long long INT64;
	typedef size_t UINT64;
	typedef float FLOAT;
	typedef bool BOOL;
	typedef char CHAR;
	typedef unsigned char UCHAR;
	typedef void VOID;

	// Positionsbezogen:
	constexpr UINT16 EN_PASSANT_WHITE_Y = 5; // �bergangene Zeile y bei En Passant f�r Spieler Wei�
	constexpr UINT16 EN_PASSANT_BLACK_Y = 2; // �bergangene Zeile y bei En Passant f�r Spieler Schwarz
	
	constexpr UINT16 PAWN_DOUBLE_MOVE_START_WHITE_Y = 6; // Startzeile y f�r wei�e Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_START_BLACK_Y = 1; // Startzeile y f�r schwarze Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_TARGET_WHITE_Y = 4; // Zielzeile y f�r wei�e Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_TARGET_BLACK_Y = 3; // Zielzeile y f�r schwarze Bauern bei Zweifelder-Bewegung

	constexpr INT16 PAWN_DIRECTION_WHITE = -1; // Laufrichtung der wei�en Bauern
	constexpr INT16 PAWN_DIRECTION_BLACK = 1;	// Laufrichtung der schwarzen Bauern

	constexpr UINT16 CASTLING_ROOK_LEFT_X = 3;	// Zielspalte x des Turms f�r lange Rochade
	constexpr UINT16 CASTLING_ROOK_RIGHT_X = 5; // Zeilspalte x des Turms f�r kurze Rochade

	constexpr UINT16 KING_START_X = 4; // Spalte x in der der K�nig startet
	constexpr UINT16 KING_CASTLING_LONG_X = 2; // Spalte x in der der K�nig nach der langen Rochade steht
	constexpr UINT16 KING_CASTLING_SHORT_X = 6; // Spalte x in der der K�nig nach der kurzen Rochade steht

	constexpr UINT16 MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES = 100; // Anzahl der Halbz�ge f�r 50-Z�ge Regel

	// Spielfeldgr��e
	constexpr UINT16 ROWS = 8;								// Zeile
	constexpr UINT16 COLUMNS = 8;							// Spalten
	constexpr UINT16 FIRST_ROW_INDEX = 0;					// Erste Zeile
	constexpr UINT16 FIRST_COLUMN_INDEX = 0;				// Erste Spalte
	constexpr UINT16 LAST_ROW_INDEX = ROWS - 1;				// Letzte Zeile
	constexpr UINT16 LAST_COLUMN_INDEX = COLUMNS - 1;		// Letzte Spalte
	constexpr UINT16 MAX_FIELDS_ON_BOARD = ROWS * COLUMNS;	// Anzahl der Zellen

	// Using f�r Container
	using BoardLine = std::array<CHAR, COLUMNS>;	// Liste aller Elemente einer Reihe in einer Zeile
	using BoardArray = std::array<BoardLine, ROWS>; // Liste aller Zeilen des Spielfelds 
	using MoveList = std::vector<Move>; // Liste an Z�gen
}