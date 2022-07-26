#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <cstdint>

#define OWL_USE_RANDOM false
#define OWL_USE_EVALUATION_COMPONENT_REPRESENTAION true
#define OWL_LOG_MOVE_STACK false
#define OWL_LOG_NO_INFO true

namespace owl
{
	// Klassen vorher deklarieren
	class Position;
	class ChessEngine;
	class ChessEvaluation;
	class ChessValidation;
	class RepitionMap;
	class FenParser;
	class IChessInterface;
	class UniversalChessInterface;
	class Command;
	class CommandBuilder;
	class EngineOptions;
	class MinMaxResult;

	struct Move;

	// Typdefinitionen
	typedef std::int16_t INT16;
	typedef std::uint16_t UINT16;
	typedef std::int32_t INT32;
	typedef std::uint32_t UINT32;
	typedef std::int64_t INT64;
	typedef std::uint64_t UINT64;
	typedef float FLOAT;
	typedef bool BOOL;
	typedef char CHAR;
	typedef unsigned char UCHAR;
	typedef void VOID;

	// Bits
	constexpr UCHAR BIT_1 = 1U;
	constexpr UCHAR BIT_2 = 2U;
	constexpr UCHAR BIT_3 = 4U;
	constexpr UCHAR BIT_4 = 8U;
	constexpr UCHAR BIT_5 = 16U;
	constexpr UCHAR BIT_6 = 32U;
	constexpr UCHAR BIT_7 = 64U;
	constexpr UCHAR BIT_8 = 128U;

	// Engine: Feature-Parameter
	constexpr UCHAR FT_NULL = 0;
	constexpr UCHAR FT_ALPHA_BETA = BIT_1;
	// Sotierungs-Parameter
	constexpr UCHAR FT_SRT_MATERIAL = BIT_2;
	constexpr UCHAR FT_SRT_MVV_LVA = BIT_3;
	constexpr UCHAR FT_SRT_KILLER = BIT_4;

	// Optionale-Paramater (nicht implementiert):
	constexpr UCHAR FT_HISTORY = BIT_5;
	constexpr UCHAR FT_PVS = BIT_6;
	constexpr UCHAR FT_NESTED = BIT_7;

	// Grenzen f�r Feature-Parameter:
	// Achtung: Features die nicht f�r die Sortierung relevant sind m�ssen vor FT_BIT_SORT_BEGIN liegen
	// Sollten optionale parameter implementiert werden, bitte diese entsprechend vorher einf�gen und < FT_BIT_SORT_BEGIN halten
	constexpr UCHAR FT_BIT_SORT_BEGIN = FT_SRT_MATERIAL;
	constexpr UCHAR FT_BIT_NOT_IMPLEMENTED_BEGIN = FT_HISTORY;

	constexpr CHAR* ENGINE_ID = "OWL-Matt 1.0";		// Schach-Engine-ID f�r UCI
	constexpr CHAR* ENGINE_AUTHOR = "TH-OWL";		// Autor:in der Schach-Engine (Hochschule)

	constexpr UCHAR FT_STANDARD = FT_ALPHA_BETA | FT_SRT_MATERIAL | FT_SRT_MVV_LVA | FT_SRT_KILLER; // OWL-Matt Standard Features

	constexpr FLOAT RANDOM_DELTA_PHI_CUT = 0.510385f; // Delta-Phi-Cut
	constexpr FLOAT RANDOM_THRESHOLD = OWL_USE_RANDOM==false ? 0 : RANDOM_DELTA_PHI_CUT; // Standardabweichung f�r alle Z�ge mit relativen Abstand zum besten gefunden Zug (Delta-Phi-Cut)
	constexpr INT32 MAX_RANDOM_MOVES_SIZE = 7; // Maximale Anzahl der erlaubten Zufallsz�ge
	// Spieler
	constexpr INT32 PLAYER_WHITE = 1;	// Spieler Wei�
	constexpr INT32 PLAYER_BLACK = -1;	// Spieler Schwarz

	// Maximalwert f�r Endstellung
	constexpr FLOAT INF = 999.0f; // Maximaler Bewertungswert (F�r Endstellungen)

	// Parameter-Konstanten
	constexpr INT32 KILLER_SIZE		= 2; // Anzahl der m�glichen Killerz�ge bezogenen auf eine Tiefe im Suchbaum
	constexpr INT32 FIRST_KILLER_INDEX = 0; // Erster Killerzug-Index
	constexpr INT32 LAST_KILLER_INDEX	= KILLER_SIZE-1; // Letzter Killerzug-Index

	constexpr INT32 KILLER_PRIO_1	= 2;	// H�chste Killerzug Priorit�t
	constexpr INT32 KILLER_PRIO_2	= 1;	// Zweite Killerzug Priorit�t
	constexpr INT32 KILLER_NO_PRIO	= 0;	// Keine Killerzug Priorit�t (Kein Killermove)
	constexpr INT32 KILLER_EMPTY	= -1;	// Noch keine Killer vorhanden?

	constexpr INT32 MAX_DEPTH = 4; // Sp�ter auf maximale erreichbare Suchtiefe anpassen

	// Positionsbezogen:
	constexpr INT32 EN_PASSANT_WHITE_Y = 5; // �bergangene Zeile y bei En Passant f�r Spieler Wei�
	constexpr INT32 EN_PASSANT_BLACK_Y = 2; // �bergangene Zeile y bei En Passant f�r Spieler Schwarz
	
	constexpr INT32 PAWN_DOUBLE_MOVE_START_WHITE_Y = 6; // Startzeile y f�r wei�e Bauern bei Zweifelder-Bewegung
	constexpr INT32 PAWN_DOUBLE_MOVE_START_BLACK_Y = 1; // Startzeile y f�r schwarze Bauern bei Zweifelder-Bewegung
	constexpr INT32 PAWN_DOUBLE_MOVE_TARGET_WHITE_Y = 4; // Zielzeile y f�r wei�e Bauern bei Zweifelder-Bewegung
	constexpr INT32 PAWN_DOUBLE_MOVE_TARGET_BLACK_Y = 3; // Zielzeile y f�r schwarze Bauern bei Zweifelder-Bewegung

	constexpr INT32 PAWN_DIRECTION_WHITE = -1; // Laufrichtung der wei�en Bauern
	constexpr INT32 PAWN_DIRECTION_BLACK = 1;	// Laufrichtung der schwarzen Bauern

	constexpr INT32 CASTLING_ROOK_LEFT_X = 3;	// Zielspalte x des Turms f�r lange Rochade
	constexpr INT32 CASTLING_ROOK_RIGHT_X = 5; // Zeilspalte x des Turms f�r kurze Rochade
	constexpr INT32 KING_START_X = 4; // Spalte x in der der K�nig startet
	constexpr INT32 KING_CASTLING_LONG_X = 2; // Spalte x in der der K�nig nach der langen Rochade steht
	constexpr INT32 KING_CASTLING_SHORT_X = 6; // Spalte x in der der K�nig nach der kurzen Rochade steht

	constexpr INT32 MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES = 100; // Anzahl der Halbz�ge f�r 50-Z�ge Regel

	// Spielfeldgr��e
	constexpr INT32 ROWS = 8;								// Zeile
	constexpr INT32 COLUMNS = 8;							// Spalten
	constexpr INT32 FIRST_ROW_INDEX = 0;					// Erste Zeile
	constexpr INT32 FIRST_COLUMN_INDEX = 0;					// Erste Spalte
	constexpr INT32 LAST_ROW_INDEX = ROWS - 1;				// Letzte Zeile
	constexpr INT32 LAST_COLUMN_INDEX = COLUMNS - 1;		// Letzte Spalte
	constexpr INT32 MAX_FIELDS_ON_BOARD = ROWS * COLUMNS;	// Anzahl der Zellen

	constexpr CHAR* STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // Start-FEN

	// Using f�r Container
	using BOARD_LINE = std::array<CHAR, COLUMNS>;	// Liste aller Elemente einer Reihe in einer Zeile
	using BOARD_ARRAY = std::array<BOARD_LINE, ROWS>; // Liste aller Zeilen des Spielfelds 
	using MOVE_LIST = std::vector<Move>; // Liste an Z�gen
	//using KILLER_LIST = Move[KILLER_SIZE][MAX_DEPTH]; // Killer-Liste f�r Killer-Heuristik

	using KILLER_LIST = std::array<std::array<Move, MAX_DEPTH>, KILLER_SIZE>; // Killerzug-Liste

	template <class A,class B>
	using PAIR = std::pair<A, B>; // Paar

	// F�r die Sortierung
	constexpr BOOL CHOOSE_LEFT = true;		// Sich bei std::sort f�r das linke Element entscheiden
	constexpr BOOL CHOOSE_RIGHT = false;	// Sich bei std::sort f�r das rechte Element entscheiden

	// Notwendige Konstanten
	constexpr INT32 WHITE_INDEX = 0; // Index f�r Spieler Wei�
	constexpr INT32 BLACK_INDEX = 1; // Index f�r Spieler Schwar

	constexpr INT32 PLAYER_COUNT = 2; // Anzahl der Spieler
	constexpr INT32 FIRST_PLAYER_INDEX = 0; // Erster Spieler-Index
	constexpr INT32 LAST_PLAYER_INDEX = PLAYER_COUNT - 1; // Lezter Spieler-Index

	// Zuordung bzw. Index f�r MATERIAL_VALUES und weitere:
	constexpr INT32 EMPTY = -1; // Leeres Feld

	constexpr INT32 PAWN_INDEX = 0; // Bauer-Index
	constexpr INT32 KNIGHT_INDEX = 1; // Springer-Index
	constexpr INT32 BISHOP_INDEX = 2; // L�ufer-Index
	constexpr INT32 ROOK_INDEX = 3; // Turm-Index
	constexpr INT32 QUEEN_INDEX = 4; // Dame-Index
	constexpr INT32 KING_INDEX = 5; // K�nig-Index

	constexpr INT32 MAX_PIECE_TYPES = 6; // Maximal m�glche Anzahl der Figurentypen
	constexpr INT32 FIRST_PIECE_TYPES_INDEX = 0; // Erster Figurentyp-Index
	constexpr INT32 LAST_PIECE_TYPES_INDEX = MAX_PIECE_TYPES - 1; // Letzter Figurentyp-Index

	constexpr INT32 MAX_PAWN_COUNT = 8; // Maximale Anzahl der m�glichen Bauern pro Spieler
	constexpr INT32 MIN_BISHOP_COUNT_PRECONDITION_BONUS = 2; // Mindestanzahl der L�ufer f�r den L�uferbonus

	// Bewertungsfunktions-Feature
	constexpr UCHAR EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE = BIT_1; // Materialwerte abh�ngig von der Spielphase (Er�ffnung, Mittel- und Endspiel)
	constexpr UCHAR EVAL_FT_PIECE_SQUARE_TABLE = BIT_2; // Piece-Square-Tabelle
	constexpr UCHAR EVAL_FT_PIECE_MOBILITY = BIT_3; // Piece-Mobility
	constexpr UCHAR EVAL_FT_PAWN_STRUCTURE = BIT_4; // Bauernstruktur (Double, Isolated, Connected, Backwards, Chain, Passed)
	constexpr UCHAR EVAL_FT_BISHOP_PAIR = BIT_5; // L�uferpaar
	// optionale/weitere Featuers
	constexpr UCHAR EVAL_FT_DYNAMIC_PAWNS = BIT_6;	// Dynamische Bauern

	// Alle Standard Bewertungsfunktions-Features
	constexpr UCHAR EVAL_FT_STANDARD =
		EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE |
		EVAL_FT_PIECE_SQUARE_TABLE |
		EVAL_FT_PIECE_MOBILITY |
		EVAL_FT_PAWN_STRUCTURE |
        EVAL_FT_BISHOP_PAIR
        | EVAL_FT_DYNAMIC_PAWNS
        ;

	// Alle Bewertungsfunktions-Features
	constexpr UCHAR EVAL_FT_ALL = 0xff;

	// Datentyp f�r Evaluierung anpassen
	// Zur �berwachung des Einflusses der Bewertungskomponenten wird das Datenobjekt einfach angepasst
	// Damit im Code nichts ver�ndert wewrden muss, werden auch alle grundlegenden boolischen Vergleichsoperationen �berladen
#if OWL_USE_EVALUATION_COMPONENT_REPRESENTAION==true
	/**
	 * Datenstruktur des Evaluierungswerts (Speichert die Werte zu jeder Bewertungskomponente mit)
	 */
	struct EvaluationDataScore
	{
		FLOAT score;			// Allgemeiner Gesamt-Score
		FLOAT material;			// Reines Material
		FLOAT gamePhase;		// Spielphasen
		FLOAT squareTable;		// PieceSquareTable-Boni/Mali
		FLOAT pieceMobility;	// Figurenmobilit�t-Boni
		FLOAT pawnStructure;	// Bauernstruktur-Boni/Mali
		FLOAT bishopPair;		// L�uferpaar-Bonus
		FLOAT dynamicPawns;		// Dynamische-Bauern

		// Boolische Operatoren �berladen:

		BOOL operator>(const EvaluationDataScore& other) { return score > other.score; } 
		BOOL operator>=(const EvaluationDataScore& other) { return score >= other.score; }
		BOOL operator<(const EvaluationDataScore& other) { return score < other.score; }
		BOOL operator<=(const EvaluationDataScore& other) { return score <= other.score; }
		BOOL operator==(const EvaluationDataScore& other) { return score == other.score; }
		BOOL operator!=(const EvaluationDataScore& other) { return score != other.score; }

		BOOL operator>	(const FLOAT& value) { return score >	value; }
		BOOL operator>=	(const FLOAT& value) { return score >= value; }
		BOOL operator<	(const FLOAT& value) { return score <	value; }
		BOOL operator<=	(const FLOAT& value) { return score <= value; }
		BOOL operator==	(const FLOAT& value) { return score == value; }
		BOOL operator!=	(const FLOAT& value) { return score != value; }

		VOID operator=(const FLOAT& value) { score = value; }

		VOID operator+=(const EvaluationDataScore& other)
		{
			score			+= other.score;
			material		+= other.material;
			gamePhase		+= other.gamePhase;
			squareTable		+= other.squareTable;
			pieceMobility	+= other.pieceMobility;
			pawnStructure	+= other.pawnStructure;
			bishopPair		+= other.bishopPair;
			dynamicPawns	+= other.dynamicPawns;
		}

		VOID operator-=(const EvaluationDataScore& other)
		{
			score -= other.score;
			material -= other.material;
			gamePhase -= other.gamePhase;
			squareTable -= other.squareTable;
			pieceMobility -= other.pieceMobility;
			pawnStructure -= other.pawnStructure;
			bishopPair -= other.bishopPair;
			dynamicPawns -= other.dynamicPawns;
		}

		// Arithmetik erlauben:

		friend EvaluationDataScore operator+(EvaluationDataScore a, const EvaluationDataScore& b)
		{
			a.score += b.score;
			a.material += b.material;
			a.gamePhase += b.gamePhase;
			a.squareTable += b.squareTable;
			a.pieceMobility += b.pieceMobility;
			a.pawnStructure += b.pawnStructure;
			a.bishopPair += b.bishopPair;
			a.dynamicPawns += b.dynamicPawns;
			return a;
		}

		friend EvaluationDataScore operator-(EvaluationDataScore a, const EvaluationDataScore& b)
		{
			a.score -= b.score;
			a.material -= b.material;
			a.gamePhase -= b.gamePhase;
			a.squareTable -= b.squareTable;
			a.pieceMobility -= b.pieceMobility;
			a.pawnStructure -= b.pawnStructure;
			a.bishopPair -= b.bishopPair;
			a.dynamicPawns -= b.dynamicPawns;
			return a;
		}

		// Score addieren
		VOID calculateScore()
		{
			score = material + gamePhase + squareTable + pieceMobility + pawnStructure + bishopPair + dynamicPawns;
		}

		operator FLOAT() const { return score; }

		// Initialisierungskonstruktor
		EvaluationDataScore()
			: score(0.0f), material(0.f), gamePhase(0.f),
			squareTable(0.f), pieceMobility(0.f),
			pawnStructure(0.f), bishopPair(0.f), dynamicPawns(0.f)
		{}

		// Konstruktor mit Score-Zuweisung
		EvaluationDataScore(FLOAT score) 
			: score(score), material(0.f), gamePhase(0.f),
			squareTable(0.f), pieceMobility(0.f),
			pawnStructure(0.f), bishopPair(0.f), dynamicPawns(0.f)
		{}
        
		// std::cout Stream Operation f�r Outputstream implementieren
        friend std::ostream& operator<<(std::ostream& os, const EvaluationDataScore& data)
        {
            os << "score: " << data.score << " material: " << data.material << " gamephase: "
            << data.gamePhase << " squaretable: " << data.squareTable << " piecemobility: " << data.pieceMobility
            <<" pawnstructure: " << data.pawnStructure << " bishoppair: " << data.bishopPair << " dynamicpawns: " << data.dynamicPawns;
            
            return os;
        }
	};

	using EVALUATION_VALUE = EvaluationDataScore; // Alias f�r Evaluierungswert auf Datenstruktur setzen

	// Print-Funktion f�r Evaluierungswert
	inline VOID PRINT_EVALUATION_VALUE(const EVALUATION_VALUE& data)
	{
		std::cout << "{";
		std::cout << " score: " << data.score;
		std::cout << " = material: " << data.material;
		std::cout << " + gamephase: " << data.gamePhase;
		std::cout << " + squaretable: " << data.squareTable;
		std::cout << " + piecemobility: " << data.pieceMobility;
		std::cout << " + pawnstructure: " << data.pawnStructure;
		std::cout << " + bishoppair: " << data.bishopPair;
		std::cout << " + dynamicpawns: " << data.dynamicPawns;
		std::cout << " }\n";
	}
#else
	using EVALUATION_VALUE = FLOAT; // Alias f�r Evaluierungswert auf FLOAT setzen

	// Print-Funktion f�r Evaluierungswert
	inline VOID PRINT_EVALUATION_VALUE(const EVALUATION_VALUE data)
	{
		std::cout << "{";
		std::cout << " score: " << data;
		std::cout << " }\n";
	}
#endif
	// Faktoren (Mit welcher Gewichtung die Features Einfluss auf die Bewertung haben 0.0 = 0% und 1.0 = 100%)
	constexpr FLOAT MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT	= 1.00f; // Materialwerte zu Spielphase (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_WEIGHT			= 0.50f; // Piece-Square-Tabelle (Faktor)
	constexpr FLOAT PIECE_MOBILITY_WEIGHT				= 2.00f; // Piece-Mobility (Faktor)
	constexpr FLOAT BISHOP_PAIR_BONUS_WEIGHT			= 0.50f; // L�uferpaar (Faktor)
	constexpr FLOAT PAWN_STRUCTURE_WEIGHT				= 0.025f; // Bauernstruktur (Faktor)

	// Materialwert + Addition f�r die jeweilige Spielphase								//  P	   N	  B		 R		Q		K
	constexpr std::array<FLOAT, MAX_PIECE_TYPES> MATERIAL_VALUES						= { 1.00f, 3.00f, 3.00f, 5.00f, 9.00f,	0.00f }; // Materialwert
	constexpr std::array<FLOAT, MAX_PIECE_TYPES> MATERIAL_ADDITION_BEGIN_GAME_PHASE		= { 0.00f, 0.25f, 0.25f, 0.00f, 0.00f,	0.00f }; // Materialwert-Addition in der Er�ffnung
	constexpr std::array<FLOAT, MAX_PIECE_TYPES> MATERIAL_ADDITION_MID_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.50f, 0.50f,	0.00f }; // Materialwert-Addition im Mittelspiel
	constexpr std::array<FLOAT, MAX_PIECE_TYPES> MATERIAL_ADDITION_END_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.75f, 0.75f,	0.00f }; // Materialwert-Addition im Endspiel

	// Spielphase Material Balance
	constexpr std::array<INT32, MAX_PIECE_TYPES> START_PIECE_COUNT = { 8,	   2,	  2,	 2,		1,		1 }; // Anzahl der Figuren beim Start (F�r 100% Materialwertberechnung)

	// Berechne die Maximale Summe
	constexpr FLOAT GET_MAX_MATERIAL_SUM(FLOAT ratio) {
		auto value = 0.0f;
		for (auto type_index = 0; type_index < MAX_PIECE_TYPES; type_index++)
		{
			value += MATERIAL_VALUES[type_index] * START_PIECE_COUNT[type_index];
		}
		return ratio * PLAYER_COUNT * value;
	};

	constexpr FLOAT MATERIAL_RATIO_FULL = 1.00f;		// Anfangsmaterialfaktor
	constexpr FLOAT MATERIAL_RATIO_MID_GAME = 0.85f;	// Mittelspielmaterialfaktor
	constexpr FLOAT MATERIAL_RATIO_END_GAME = 0.50f;	// Endspielmaterialfaktor

	constexpr FLOAT MAX_MATERIAL_SUM = GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_FULL);				// Er�ffnungs-Material-Summe
	constexpr FLOAT MAX_MATERIAL_SUM_MID_GAME = GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_MID_GAME);	// Mittelspiel-Material-Summe
	constexpr FLOAT MAX_MATERIAL_SUM_END_GAME = GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_END_GAME);	// Endspiel-Material-Summe

	// Wei�e Figuren:
	constexpr CHAR WHITE_PAWN = 'P';
	constexpr CHAR WHITE_KNIGHT = 'N';
	constexpr CHAR WHITE_BISHOP = 'B';
	constexpr CHAR WHITE_ROOK = 'R';
	constexpr CHAR WHITE_QUEEN = 'Q';
	constexpr CHAR WHITE_KING = 'K';

	// Schwarze Figuren:
	constexpr CHAR BLACK_PAWN = 'p';
	constexpr CHAR BLACK_KNIGHT = 'n';
	constexpr CHAR BLACK_BISHOP = 'b';
	constexpr CHAR BLACK_ROOK = 'r';
	constexpr CHAR BLACK_QUEEN = 'q';
	constexpr CHAR BLACK_KING = 'k';

	// Sonstige Eigenschaften
	constexpr UCHAR EMPTY_FIELD = ' ';

	constexpr INT32 MAX_PIECE_PROMOTION_TYPES = 4; // Anzahl der verschiedenen Figuren, die aus dem Bauern umgewandelt werden k�nnen.

	// Liste der wei�en Figuren
	constexpr CHAR WHITE_PIECES[MAX_PIECE_TYPES+1] =
	{
		WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, 
		WHITE_ROOK, WHITE_QUEEN, WHITE_KING, '\0'
	};

	// Liste der schwarzen Figuren
	constexpr CHAR BLACK_PIECES[MAX_PIECE_TYPES+1] =
	{ 
		BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, 
		BLACK_ROOK, BLACK_QUEEN, BLACK_KING, '\0'
	};

	// Liste der wei�en Bauernumwandlungsfiguren
	constexpr CHAR WHITE_PROMOTION_PIECES[MAX_PIECE_PROMOTION_TYPES] =
	{
		WHITE_KNIGHT, WHITE_BISHOP,
		WHITE_ROOK, WHITE_QUEEN
	};

	// Liste der schwarezn Bauernumwandlungsfiguren
	constexpr CHAR BLACK_PROMOTION_PIECES[MAX_PIECE_PROMOTION_TYPES] =
	{
		BLACK_KNIGHT, BLACK_BISHOP,
		BLACK_ROOK, BLACK_QUEEN,
	};

	// Liste der Figuren
	constexpr CHAR PIECES[PLAYER_COUNT][MAX_PIECE_TYPES] =
	{
		{WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING},
		{BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING}
	};

	// Gebe den Figurtyp-Index aus den Figurenstring aus
	constexpr INT32 GET_PIECE_INDEX_BY_TYPE(CHAR piece)
	{
		switch (piece)
		{
		case WHITE_PAWN:
		case BLACK_PAWN:
			return PAWN_INDEX;
			break;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			return KNIGHT_INDEX;
			break;
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			return BISHOP_INDEX;
			break;
		case WHITE_ROOK:
		case BLACK_ROOK:
			return ROOK_INDEX;
			break;
		case WHITE_QUEEN:
		case BLACK_QUEEN:
			return QUEEN_INDEX;
			break;
		case WHITE_KING:
		case BLACK_KING:
			return KING_INDEX;
			break;
		default:
			return EMPTY;
			break;
		}
	}

	// Gebe den Spieler-Index zu dem korrespondieren Figurenstring
	constexpr INT32 GET_PLAYER_INDEX_BY_PIECE(CHAR piece)
	{
		switch (piece)
		{
		case WHITE_PAWN:
		case WHITE_KNIGHT:
		case WHITE_BISHOP:
		case WHITE_ROOK:
		case WHITE_QUEEN:
		case WHITE_KING:
			return WHITE_INDEX;
			break;
		case BLACK_PAWN:
		case BLACK_KNIGHT:
		case BLACK_BISHOP:
		case BLACK_ROOK:
		case BLACK_QUEEN:
		case BLACK_KING:
			return BLACK_INDEX;
			break;
		default:
			return EMPTY_FIELD;
			break;
		}
	}

	constexpr FLOAT BISHOP_PAIR_BONUS = 0.50f; // L�uferpaar-Bonus

	// Figurenbewegung (Gewichtung)
	constexpr FLOAT PIECE_MOBILITY_PAWN_WEIGHT		= 0.10f; // Bauern-Figurbewegung (Faktor)
	constexpr FLOAT PIECE_MOBILITY_KNIGHT_WEIGHT	= 0.10f; // Springer-Figurenbewegung (Faktor)
	constexpr FLOAT PIECE_MOBILITY_BISHOP_WEIGHT	= 0.10f; // L�ufer-Figurenbewegung (Faktor)
	constexpr FLOAT PIECE_MOBILITY_ROOK_WEIGHT		= 0.10f; // Turm-Figurenbewegung (Faktor)
	constexpr FLOAT PIECE_MOBILITY_QUEEN_WEIGHT		= 0.10f; // Dame-Figurenbewegung (Faktor)
	constexpr FLOAT PIECE_MOBILITY_KING_WEIGHT		= 0.10f; // K�nig-Figurenbewegung (Faktor)

	// Zus�tzliche Dynamische Bauerngewichtung
	constexpr INT32 MAX_DYNAMIC_PAWNS = MAX_PAWN_COUNT; // Anzahl der m�glichen Bauern f�r die Dynamik
	constexpr INT32 DYNAMIC_PAWNS_LAST_INDEX = MAX_DYNAMIC_PAWNS - 1; // Index der letzten Bauerndynamik

	constexpr std::array<FLOAT, MAX_DYNAMIC_PAWNS> MATERIAL_DYNAMIC_PAWNS	= { 0.10f, 0.05f, 0.025f, 0.00f, 0.00f, -0.01f, -0.02f, -0.025f }; // Dynamische additive Bauerngewichtung je nach Anzahl der verbleibenden Bauern
	constexpr std::array<FLOAT, ROWS> PASSED_PAWNS_PROGRESS_BONUS			= { 0.00f, 0.00f,  0.00f, 0.00f, 0.05f,  0.15f,  0.35f,   0.00f }; // Additiver Bonus f�r Freibauern je nach Entfernung zur gegnerischen Grundreihe

	// Hinweis: Alle Mali und Boni werden pro Bauer auf dem Spielfeld gegeben
	// Bauernstruktur Malus
	constexpr FLOAT PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY = -0.200f;	// Doppelte Bauern Malus
	constexpr FLOAT PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY = -0.100f;	// Isolierte Bauern Malus
	constexpr FLOAT PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY = -0.125f;	// R�ckst�ndige Bauern Malus
	constexpr FLOAT PAWN_STRUCTURE_ISOLATED_DOUBLE_PAWNS_PENALTY = -0.200f; // Doppelte Bauern, die isoliert sind

	// Bauernstruktur Bonus
	constexpr FLOAT PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS = 0.100f;	// Verbundene Bauern Bonus
	constexpr FLOAT PAWN_STRUCTURE_CHAIN_PAWNS_BONUS = 0.100f;	// Bauernkette Bonus
	constexpr FLOAT PAWN_STRUCTURE_PASSED_PAWNS_BONUS = 0.300f;	// Freibauern Bonus
	constexpr FLOAT PAWN_STRUCTURE_CONNECTED_PASSED_PAWNS_BONUS = 0.700f; // Freibauern, die verbunden sind
	// Piece Square Tables
	constexpr FLOAT PIECE_SQUARE_TABLE_PAWN_WEIGHT = 1.0f;	// Bauern-Tabellen-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_KNIGHT_WEIGHT = 1.0f;	// Springer-Tabellen-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_BISHOP_WEIGHT = 1.0f;	// L�ufer-Tabellen-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_ROOK_WEIGHT = 1.0f;	// Turm-Tabellen-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_QUEEN_WEIGHT = 1.0f;	// Dame-Tabellen-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT = 1.0f;	// K�nig-Tabellen-Mittelspiel-Gewicht (Faktor)
	constexpr FLOAT PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT = 1.0f;	// K�nig-Tabellen-Endspiel-Gewicht (Faktor)

	// Piece-Square-Tabelle der Bauern
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_PAWN = {
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
		0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f,
		0.10f, 0.10f, 0.20f, 0.30f, 0.30f, 0.20f, 0.10f, 0.10f,
		0.05f, 0.05f, 0.10f, 0.25f, 0.25f, 0.10f, 0.05f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.20f, 0.20f, 0.00f, 0.00f, 0.00f,
		0.05f,-0.05f,-0.10f, 0.00f, 0.00f,-0.10f,-0.05f, 0.05f,
		0.05f, 0.10f, 0.10f,-0.20f,-0.20f, 0.10f, 0.10f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f
	};

	// Piece-Square-Tabelle der Springer
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KNIGHT = {
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f,
		-0.40f,-0.20f, 0.00f, 0.00f, 0.00f, 0.00f,-0.20f,-0.40f,
		-0.30f, 0.00f, 0.10f, 0.15f, 0.15f, 0.10f, 0.00f,-0.30f,
		-0.30f, 0.05f, 0.15f, 0.20f, 0.20f, 0.15f, 0.05f,-0.30f,
		-0.30f, 0.00f, 0.15f, 0.20f, 0.20f, 0.15f, 0.00f,-0.30f,
		-0.30f, 0.05f, 0.10f, 0.15f, 0.15f, 0.10f, 0.05f,-0.30f,
		-0.40f,-0.20f, 0.00f, 0.05f, 0.05f, 0.00f,-0.20f,-0.40f,
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f
	};

	// Piece-Square-Tabelle der L�ufer
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_BISHOP = {
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f, 0.10f, 0.10f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.05f, 0.05f, 0.10f, 0.10f, 0.05f, 0.05f,-0.10f,
		-0.10f, 0.00f, 0.10f, 0.10f, 0.10f, 0.10f, 0.00f,-0.10f,
		-0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,-0.10f,
		-0.10f, 0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.05f,-0.10f,
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f
	};

	// Piece-Square-Tabelle des Turms
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_ROOK = {
		 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,  0.00f,
		 0.05f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,  0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		 0.00f, 0.00f, 0.00f, 0.05f, 0.05f, 0.00f, 0.00f,  0.00f
	};

	// Piece-Square-Tabelle der Dame
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_QUEEN = {
		-0.20f,-0.10f,-0.10f, -0.05f, -0.05f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f,  0.00f,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.05f,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.05f, 0.00f, 0.05f,  0.05f,  0.05f, 0.05f, 0.00f,-0.05f,
		 0.00f, 0.00f, 0.05f,  0.05f,  0.05f, 0.05f, 0.00f,-0.05f,
		-0.10f, 0.05f, 0.05f,  0.05f,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.00f,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.20f,-0.10f,-0.10f, -0.05f, -0.05f,-0.10f,-0.10f,-0.20f
	};

	// Piece-Square-Tabelle des K�nigs zum Mittelspiel
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KING_MID_GAME = {
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.20f,-0.30f,-0.30f,-0.40f,-0.40f,-0.30f,-0.30f,-0.20f,
		-0.10f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.10f,
		 0.20f, 0.20f, 0.00f, 0.00f, 0.00f, 0.00f, 0.20f, 0.20f,
		 0.20f, 0.30f, 0.10f, 0.00f, 0.00f, 0.10f, 0.30f, 0.20f
	};

	// Piece-Square-Tabelle des K�nigs zum Endspiels
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KING_END_GAME = {
		-0.50f,-0.40f,-0.30f,-0.20f,-0.20f,-0.30f,-0.40f,-0.50f,
		-0.30f,-0.20f,-0.10f, 0.00f, 0.00f,-0.10f,-0.20f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.30f, 0.00f, 0.00f, 0.00f, 0.00f,-0.30f,-0.30f,
		-0.50f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.50f
	};

	// Funktion zur Spiegelung der Piece-Square-Tables (F�r Spieler Schwarz)
	constexpr std::array<FLOAT, MAX_FIELDS_ON_BOARD> MIRROR_PIECE_SQUARE_TABLE(const std::array<FLOAT, MAX_FIELDS_ON_BOARD> t)
	{
		std::array<FLOAT, MAX_FIELDS_ON_BOARD>&& sort_table =
		{
			t[56], t[57], t[58], t[59], t[60], t[61], t[62], t[63],
			t[48], t[49], t[50], t[51], t[52], t[53], t[54], t[55],
			t[40], t[41], t[42], t[43], t[44], t[45], t[46], t[47],
			t[32], t[33], t[34], t[35], t[36], t[37], t[38], t[39],
			t[24], t[25], t[26], t[27], t[28], t[29], t[30], t[31],
			t[16], t[17], t[18], t[19], t[20], t[21], t[22], t[23],
			 t[8],  t[9], t[10], t[11], t[12], t[13], t[14], t[15],
			 t[0],  t[1],  t[2],  t[3],  t[4],  t[5],  t[6],  t[7]
		};

		return sort_table;
	}

	/**
	 * M�gliche Caputres in Schach.
	 */
	enum class Captures
	{
		kxP, kxN, kxB, kxR, kxQ,
		qxP, qxN, qxB, qxR, qxQ,
		rxP, rxN, rxB, rxR, rxQ,
		bxP, bxN, bxB, bxR, bxQ,
		nxP, nxN, nxB, nxR, nxQ,
		pxP, pxN, pxB, pxR, pxQ
	};

	/**
	 * String zu den m�glichen Captures in Schach. (F�r Logging)
	 */
	static constexpr std::array<CHAR*, MAX_PIECE_TYPES* (MAX_PIECE_TYPES - 1)> s_capture_map =
	{
		"kxP","kxN","kxB","kxR","kxQ",
		"qxP","qxN","qxB","qxR","qxQ",
		"rxP","rxN","rxB","rxR","rxQ",
		"bxP","bxN","bxB","bxR","bxQ",
		"nxP","nxN","nxB","nxR","nxQ",
		"pxP","pxN","pxB","pxR","pxQ"
	};

	// Maximale Z�ge
	constexpr INT32 MAX_MOVES_PER_PLY_BOUND = 64; // Maximal legale Z�ge pro Zug (Wird f�r Speicherallokation eines std::vector verwendet)
	constexpr INT32 MAX_MOVES_PER_KNIGHT = 8; // Maximale Anzahl der Z�ge f�r einen Springer
	constexpr INT32 MAX_MOVES_PER_BISHOP = 13; // Maximale Anzahl der Z�ge f�r einen L�ufer
	constexpr INT32 MAX_MOVES_PER_ROOK = 14; // Maximale Anzahl der Z�ge f�r einen Turm

	constexpr INT32 MAX_MOVES_PER_QUEEN = MAX_MOVES_PER_BISHOP + MAX_MOVES_PER_ROOK; // Maximale Anzahl der Z�ge f�r eine Dame
	constexpr INT32 MAX_MOVES_PER_KING = 10; // Maximale Anzahl der Z�ge f�r den K�nig
	constexpr INT32 MAX_MOVES_PER_KING_WITHOUT_CASTLING = MAX_MOVES_PER_KING - 2; // Maximale Anzahl der Z�ge f�r einen K�nig, ohne Rochade

	constexpr INT32 MAX_ROOK_DIR = 4; // Maximale Laufrichtungen des Turms
	constexpr INT32 MAX_BISHOP_DIR = 4; // Maximale Laufrichtungen des L�ufers

	constexpr INT32 VEC = 2; // Vector2
	constexpr INT32 FIRST = 0; // Erstes Element (Index)
	constexpr INT32 SECOND = 1; // Zweites Element (Index)

	constexpr INT32 ONE_FIELD_DOWN = 1; // Ein Feld nach unten (y+1)
	constexpr INT32 ONE_FIELD_UP = -1; // Ein Feld nach oben (y-1)

	// Laufrichtungen des Springers
	constexpr INT32 MOVE_DIR_KNIGHT[MAX_MOVES_PER_KNIGHT][VEC] = 
	{ 
		{-1,-2}, {2,-1}, {2,1}, {1,2}, {-1,2}, {-2,1}, {-2,-1}, {1,-2}
	};

	// Laufrichtungen des K�nigs
	constexpr INT32 MOVE_DIR_KING[MAX_MOVES_PER_KING_WITHOUT_CASTLING][VEC] =
	{
		{0,-1}, {1,-1}, {1,0}, {1,1}, {0,1},{-1,1}, {-1,0}, {-1,-1}
	};

	// Laufrichtungen des Turms
	constexpr INT32 MOVE_DIR_ROOK[MAX_ROOK_DIR][VEC] =
	{
		{0,-1}, {1,0}, {0,1}, {-1,0}
	};

	// Laufrichtung des L�ufers
	constexpr INT32 MOVE_DIR_BISHOP[MAX_BISHOP_DIR][VEC] =
	{
		{-1,-1}, {1,-1}, {1,1}, {-1,1}
	};
}
