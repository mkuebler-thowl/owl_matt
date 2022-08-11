#pragma once

#include <array>
#include <vector>

namespace owl
{
	// Klassen deklarieren (Für using benötigt)
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
	constexpr UINT16 EN_PASSANT_WHITE_Y = 5; // Übergangene Zeile y bei En Passant für Spieler Weiß
	constexpr UINT16 EN_PASSANT_BLACK_Y = 2; // Übergangene Zeile y bei En Passant für Spieler Schwarz
	
	constexpr UINT16 PAWN_DOUBLE_MOVE_START_WHITE_Y = 6; // Startzeile y für weiße Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_START_BLACK_Y = 1; // Startzeile y für schwarze Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_TARGET_WHITE_Y = 4; // Zielzeile y für weiße Bauern bei Zweifelder-Bewegung
	constexpr UINT16 PAWN_DOUBLE_MOVE_TARGET_BLACK_Y = 3; // Zielzeile y für schwarze Bauern bei Zweifelder-Bewegung

	constexpr INT16 PAWN_DIRECTION_WHITE = -1; // Laufrichtung der weißen Bauern
	constexpr INT16 PAWN_DIRECTION_BLACK = 1;	// Laufrichtung der schwarzen Bauern

	constexpr UINT16 CASTLING_ROOK_LEFT_X = 3;	// Zielspalte x des Turms für lange Rochade
	constexpr UINT16 CASTLING_ROOK_RIGHT_X = 5; // Zeilspalte x des Turms für kurze Rochade

	constexpr UINT16 KING_START_X = 4; // Spalte x in der der König startet
	constexpr UINT16 KING_CASTLING_LONG_X = 2; // Spalte x in der der König nach der langen Rochade steht
	constexpr UINT16 KING_CASTLING_SHORT_X = 6; // Spalte x in der der König nach der kurzen Rochade steht

	constexpr UINT16 MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES = 100; // Anzahl der Halbzüge für 50-Züge Regel

	// Spielfeldgröße
	constexpr UINT16 ROWS = 8;								// Zeile
	constexpr UINT16 COLUMNS = 8;							// Spalten
	constexpr UINT16 FIRST_ROW_INDEX = 0;					// Erste Zeile
	constexpr UINT16 FIRST_COLUMN_INDEX = 0;				// Erste Spalte
	constexpr UINT16 LAST_ROW_INDEX = ROWS - 1;				// Letzte Zeile
	constexpr UINT16 LAST_COLUMN_INDEX = COLUMNS - 1;		// Letzte Spalte
	constexpr UINT16 MAX_FIELDS_ON_BOARD = ROWS * COLUMNS;	// Anzahl der Zellen

	// Using für Container
	using BoardLine = std::array<CHAR, COLUMNS>;	// Liste aller Elemente einer Reihe in einer Zeile
	using BoardArray = std::array<BoardLine, ROWS>; // Liste aller Zeilen des Spielfelds 
	using MoveList = std::vector<Move>; // Liste an Zügen
}