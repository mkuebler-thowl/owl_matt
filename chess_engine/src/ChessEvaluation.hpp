#pragma once

#include <array>
#include "Position.hpp"

namespace matt
{
	// Notwendige Konstanten
	constexpr unsigned short WHITE = 0;
	constexpr unsigned short BLACK = 1;

	constexpr unsigned short PLAYER_COUNT = 2;
	constexpr unsigned short FIRST_PLAYER_INDEX = 0;
	constexpr unsigned short LAST_PLAYER_INDEX = PLAYER_COUNT - 1;

	// Zuordung bzw. Index für MATERIAL_VALUES und weitere:
	constexpr unsigned short EMPTY = -1; // Leeres Feld

	constexpr unsigned short PAWN = 0; // Bauer-Index
	constexpr unsigned short KNIGHT = 1; // Springer-Index
	constexpr unsigned short BISHOP = 2; // Läufer-Index
	constexpr unsigned short ROOK = 3; // Turm-Index
	constexpr unsigned short QUEEN = 4; // Dame-Index
	constexpr unsigned short KING = 5; // König-Index

	constexpr unsigned short MAX_PIECE_TYPES = 6;
	constexpr unsigned short FIRST_PIECE_TYPES_INDEX = 0;
	constexpr unsigned short LAST_PIECE_TYPES_INDEX = MAX_PIECE_TYPES - 1;

	constexpr unsigned short MAX_PAWN_COUNT = 8;
	constexpr unsigned short MIN_BISHOP_COUNT_PRECONDITION_BONUS = 2;

	// Bewertungsfunktions-Feature
	constexpr unsigned char EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE = (1 << 0); // Materialwerte abhängig von der Spielphase (Eröffnung, Mittel- und Endspiel)
	constexpr unsigned char EVAL_FT_PIECE_SQUARE_TABLE			= (1 << 1); // Piece-Square-Tabelle
	constexpr unsigned char EVAL_FT_PIECE_MOBILITY				= (1 << 2); // Piece-Mobility
	constexpr unsigned char EVAL_FT_PAWN_STRUCTURE				= (1 << 3); // Bauernstruktur (Double, Isolated, Connected, Backwards, Chain, Passed)
	constexpr unsigned char EVAL_FT_BISHOP_PAIR					= (1 << 4); // Läuferpaar
	// optionale/weitere Featuers
	constexpr unsigned char EVAL_FT_DYNAMIC_PAWNS = (1 << 5);	// Dynamische Bauern

	// Alle Standard Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_STANDARD = 
		EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE | 
		EVAL_FT_PIECE_SQUARE_TABLE | 
		EVAL_FT_PIECE_MOBILITY | 
		EVAL_FT_PAWN_STRUCTURE |
		EVAL_FT_BISHOP_PAIR;

	// Alle Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_ALL = 0xff;

	// Faktoren (Mit welcher Gewichtung die Features Einfluss auf die Bewertung haben 0.0 = 0% und 1.0 = 100%)

	constexpr float MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT	= 1.00f; // Materialwerte zu Spielphase (Faktor)
	constexpr float PIECE_SQUARE_TABLE_WEIGHT			= 1.00f; // Piece-Square-Tabelle (Faktor)
	constexpr float PIECE_MOBILITY_WEIGHT				= 1.00f; // Piece-Mobility (Faktor)
	constexpr float BISHOP_PAIR_BONUS_WEIGHT			= 1.00f; // Bauernstruktur (Faktor)
	constexpr float PAWN_STRUCTURE_WEIGHT				= 1.00f; // Läuferpaar (Faktor)


	// Materialwert + Addition für die jeweilige Spielphase								//  P	   N	  B		 R		Q		K
	constexpr std::array<float, MAX_PIECE_TYPES> MATERIAL_VALUES						= { 1.00f, 3.00f, 3.00f, 5.00f, 9.00f,	0.00f }; // Materialwert
	constexpr std::array<float, MAX_PIECE_TYPES> MATERIAL_ADDITION_BEGIN_GAME_PHASE		= { 0.00f, 0.25f, 0.25f, 0.00f, 0.00f,	0.00f }; // Materialwert-Addition in der Eröffnung
	constexpr std::array<float, MAX_PIECE_TYPES> MATERIAL_ADDITION_MID_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.50f, 0.50f,	0.00f }; // Materialwert-Addition im Mittelspiel
	constexpr std::array<float, MAX_PIECE_TYPES> MATERIAL_ADDITION_END_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.75f, 0.75f,	0.00f }; // Materialwert-Addition im Endspiel

	// Spielphase Material Balance
	constexpr std::array<unsigned short, MAX_PIECE_TYPES> START_PIECE_COUNT				= { 8,	   2,	  2,	 2,		1,		1	  }; // Anzahl der Figuren beim Start (Für 100% Materialwertberechnung)
	constexpr float GET_MAX_MATERIAL_SUM(float ratio) {
		auto value = 0.0f;
		for (auto type_index = 0; type_index < MAX_PIECE_TYPES; type_index++)
		{
			value += MATERIAL_VALUES[type_index] * START_PIECE_COUNT[type_index];
		}
		return ratio * PLAYER_COUNT * value;
	};

	constexpr float MATERIAL_RATIO_FULL		= 1.00f;
	constexpr float MATERIAL_RATIO_MID_GAME = 0.85f;
	constexpr float MATERIAL_RATIO_END_GAME = 0.50f;

	constexpr float MAX_MATERIAL_SUM			= GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_FULL);		// Eröffnungs-Material-Summe
	constexpr float MAX_MATERIAL_SUM_MID_GAME	= GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_MID_GAME);	// Mittelspiel-Material-Summe
	constexpr float MAX_MATERIAL_SUM_END_GAME	= GET_MAX_MATERIAL_SUM(MATERIAL_RATIO_END_GAME);	// Endspiel-Material-Summe

	// Weiße Figuren:
	constexpr unsigned char PAWN_WHITE = 'P';
	constexpr unsigned char KNIGHT_WHITE = 'N';
	constexpr unsigned char BISHOP_WHITE = 'B';
	constexpr unsigned char ROOK_WHITE = 'R';
	constexpr unsigned char QUEEN_WHITE = 'Q';
	constexpr unsigned char KING_WHITE = 'K';

	// Schwarze Figuren:
	constexpr unsigned char PAWN_BLACK = 'p';
	constexpr unsigned char KNIGHT_BLACK = 'n';
	constexpr unsigned char BISHOP_BLACK = 'b';
	constexpr unsigned char ROOK_BLACK = 'r';
	constexpr unsigned char QUEEN_BLACK = 'q';
	constexpr unsigned char KING_BLACK = 'k';

	// Sonstige Eigenschaften
	constexpr unsigned char EMPTY_PLACE = ' ';

	constexpr unsigned char PIECES[PLAYER_COUNT][MAX_PIECE_TYPES] =
	{
		{PAWN_WHITE, KNIGHT_WHITE, BISHOP_WHITE,  ROOK_WHITE, QUEEN_WHITE, KING_WHITE},
		{PAWN_BLACK, KNIGHT_BLACK, BISHOP_BLACK, ROOK_BLACK, QUEEN_BLACK, KING_BLACK}
	};

	constexpr unsigned short GET_PIECE_INDEX_BY_TYPE(unsigned char piece)
	{
		switch (piece)
		{
		case PAWN_WHITE:
		case PAWN_BLACK:
			return PAWN;
			break;
		case KNIGHT_WHITE:
		case KNIGHT_BLACK:
			return KNIGHT;
			break;
		case BISHOP_WHITE:
		case BISHOP_BLACK:
			return BISHOP;
			break;
		case ROOK_WHITE:
		case ROOK_BLACK:
			return ROOK;
			break;
		case QUEEN_WHITE:
		case QUEEN_BLACK:
			return QUEEN;
			break;
		case KING_WHITE:
		case KING_BLACK:
			return KING;
			break;
		default:
			return EMPTY;
			break;
		}
	}

	constexpr unsigned short GET_PIECE_COLOR_BY_TYPE(unsigned char piece)
	{
		switch (piece)
		{
		case PAWN_WHITE:
		case KNIGHT_WHITE:
		case BISHOP_WHITE:
		case ROOK_WHITE:
		case QUEEN_WHITE:
		case KING_WHITE:
			return WHITE;
			break;
		case PAWN_BLACK:
		case KNIGHT_BLACK:
		case BISHOP_BLACK:
		case ROOK_BLACK:
		case QUEEN_BLACK:
		case KING_BLACK:
			return BLACK;
			break;
		default:
			return EMPTY_PLACE;
			break;
		}
	}

	// Bonus
	constexpr float BISHOP_PAIR_BONUS = 0.50f; // Läuferpaar-Bonus

	// Figurenbewegung (Gewichtung)
	constexpr float PIECE_MOBILITY_PAWN_WEIGHT		= 0.10f; // Bauern-Figurbewegung (Faktor)
	constexpr float PIECE_MOBILITY_KNIGHT_WEIGHT	= 0.10f; // Springer-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_BISHOP_WEIGHT	= 0.10f; // Läufer-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_ROOK_WEIGHT		= 0.10f; // Turm-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_QUEEN_WEIGHT		= 0.10f; // Dame-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_KING_WEIGHT		= 0.10f; // König-Figurenbewegung (Faktor)

	// Zusätzliche Dynamische Bauerngewichtung
	constexpr unsigned short MAX_DYNAMIC_PAWNS = MAX_PAWN_COUNT;
	constexpr unsigned short DYNAMIC_PAWNS_LAST_INDEX = MAX_DYNAMIC_PAWNS - 1;

	constexpr std::array<float, MAX_DYNAMIC_PAWNS> MATERIAL_DYNAMIC_PAWNS = { 0.05f, 0.03f, 0.01f, 0.00f, -0.01f, -0.02f, -0.03f, -0.05f };


	// Bauernstruktur Malus
	constexpr float PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY		= -0.200f;	// Doppelte Bauern Malus
	constexpr float PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY	= -0.100f;	// Isolierte Bauern Malus
	constexpr float PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY	= -0.125f;	// Rückständige Bauern Malus
	// Bauernstruktur Bonus
	constexpr float PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS	= 0.100f;	// Verbundene Bauern Bonus
	constexpr float PAWN_STRUCTURE_CHAIN_PAWNS_BONUS		=  0.100f;	// Bauernkette Bonus
	constexpr float PAWN_STRUCTURE_PASSED_PAWNS_BONUS		=  0.200f;	// Freibauern Bonus

	// Piece Square Tables

	constexpr float PIECE_SQUARE_TABLE_PAWN_WEIGHT			= 1.0f;	// Bauern-Tabellen-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_KNIGHT_WEIGHT		= 1.0f;	// Springer-Tabellen-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_BISHOP_WEIGHT		= 1.0f;	// Läufer-Tabellen-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_ROOK_WEIGHT			= 1.0f;	// Turm-Tabellen-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_QUEEN_WEIGHT			= 1.0f;	// Dame-Tabellen-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT	= 1.0f;	// König-Tabellen-Mittelspiel-Gewicht (Faktor)
	constexpr float PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT	= 1.0f;	// König-Tabellen-Endspiel-Gewicht (Faktor)

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_PAWN = {
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
		0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f,
		0.10f, 0.10f, 0.20f, 0.30f, 0.30f, 0.20f, 0.10f, 0.10f,
		0.05f, 0.05f, 0.10f, 0.25f, 0.25f, 0.10f, 0.05f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.20f, 0.20f, 0.00f, 0.00f, 0.00f,
		0.05f,-0.05f,-0.10f, 0.00f, 0.00f,-0.10f,-0.05f, 0.05f,
		0.05f, 0.10f, 0.10f,-0.20f,-0.20f, 0.10f, 0.10f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KNIGHT = {
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f,
		-0.00f,-0.20f, 0.00f, 0.00f, 0.00f, 0.00f,-0.20f,-0.40f,
		-0.00f, 0.00f, 0.10f, 0.15f, 0.15f, 0.10f, 0.00f,-0.30f,
		-0.00f, 0.05f, 0.15f, 0.20f, 0.20f, 0.15f, 0.05f,-0.30f,
		-0.00f, 0.00f, 0.15f, 0.20f, 0.20f, 0.15f, 0.00f,-0.30f,
		-0.00f, 0.05f, 0.10f, 0.15f, 0.15f, 0.10f, 0.05f,-0.30f,
		-0.40f,-0.20f, 0.00f, 0.05f, 0.05f, 0.00f,-0.20f,-0.40f,
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_BISHOP = {
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f, 0.10f, 0.10f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.05f, 0.05f, 0.10f, 0.10f, 0.05f, 0.05f,-0.10f,
		-0.10f, 0.00f, 0.10f, 0.10f, 0.10f, 0.10f, 0.00f,-0.10f,
		-0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,-0.10f,
		-0.10f, 0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.05f,-0.10f,
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_ROOK = {
		 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,  0.00f,
		 0.05f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,  0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		 0.00f, 0.00f, 0.00f, 0.05f, 0.05f, 0.00f, 0.00f,  0.00f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_QUEEN = {
		-0.20f,-0.10f,-0.10f, -0.05, -0.05f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f,  0.00,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.05f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.05f,
		 0.00f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.05f,
		-0.10f, 0.05f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.00,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.20f,-0.10f,-0.10f, -0.05, -0.05f,-0.10f,-0.10f,-0.20f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KING_MID_GAME = {
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.20f,-0.30f,-0.30f,-0.40f,-0.40f,-0.30f,-0.30f,-0.20f,
		-0.10f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.10f,
		 0.20f, 0.20f, 0.00f, 0.00f, 0.00f, 0.00f, 0.20f, 0.20f,
		 0.20f, 0.30f, 0.10f, 0.00f, 0.00f, 0.10f, 0.30f, 0.20f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> PIECE_SQUARE_TABLE_KING_END_GAME = {
		-0.50f,-0.40f,-0.30f,-0.20f,-0.20f,-0.30f,-0.40f,-0.50f,
		-0.30f,-0.20f,-0.10f, 0.00f, 0.00f,-0.10f,-0.20f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.30f, 0.00f, 0.00f, 0.00f, 0.00f,-0.30f,-0.30f,
		-0.50f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.50f
	};

	constexpr std::array<float, MAX_FIELDS_ON_BOARD> MIRROR_PIECE_SQUARE_TABLE(const std::array<float, MAX_FIELDS_ON_BOARD> t)
	{
		std::array<float, MAX_FIELDS_ON_BOARD>&& sort_table =
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

	class ChessEvaluation
	{
	public:
		/// <summary>
		/// Bewertungsfunktion
		/// </summary>
		/// <param name="position">Ausgangstellung bzw. Position, die bewertet werden soll</param>
		/// <param name="enginePlayer">Der Spieler (Weiß/Schwarz), welcher von der Schach-Engine gespielt wird</param>
		/// <param name="evaluationFeatureFlags">Feature-Parameter-Flags für die Komponenten der Bewertungsfunktion</param>
		/// <returns>Nutzwert der Bewertungsfunktion</returns>
		static float evaluate(const Position& position, short enginePlayer, unsigned char evaluationFeatureFlags = 0);
	private:
		/// Doppelte Bauern?
		static bool isDoublePawn(const Position& position, int x, int y);
		/// Verbundene Bauern?
		static bool isConnectedPawn(const Position& position, int x, int y);
		/// Backwards Bauern?
		static bool isBackwardsPawn(const Position& position, int x, int y);
		/// Passierte Bauern?
		static bool isPassedPawn(const Position& position, int x, int y);
		/// Verkettete Bauern? 
		static bool isChainPawn(const Position& position, int x, int y);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein freundlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static bool isPieceEqualOnOffset(const Position& position, int x, int y, int xOffset, int yOffset);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein feindlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static bool isPieceEnemyPawnOnOffset(const Position& position, int x, int y, int xOffset, int yOffset);
	public:

		static unsigned char GetPlayerIndexByPositionPlayer(short currentPlayerOfPosition);
		static unsigned char GetEnemyPiece(short currentPlayerOfPosition, unsigned short pieceIndex);
	};
}