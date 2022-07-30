#pragma once

#include <array>
#include "Position.hpp"

namespace matt
{
	// Bewertungsfunktions-Feature
	constexpr unsigned char EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE = (1 << 0); // Materialwerte abhängig von der Spielphase (Eröffnung, Mittel- und Endspiel)
	constexpr unsigned char EVAL_FT_PIECE_SQUARE_TABLE			= (1 << 1); // Piece-Square-Tabelle
	constexpr unsigned char EVAL_FT_PIECE_MOBILITY				= (1 << 2); // Piece-Mobility
	constexpr unsigned char EVAL_FT_PAWN_STRUCUTRE				= (1 << 3); // Bauernstruktur (Double, Isolated, Connected, Backwards, Chain, Passed)
	constexpr unsigned char EVAL_FT_BISHOP_PAIR					= (1 << 4); // Läuferpaar
	// optionale/weitere Featuers
	constexpr unsigned char EVAL_FT_DYNAMIC_PAWNS = (1 << 5);	// Dynamische Bauern

	// Alle Standard Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_STANDARD = 
		EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE | 
		EVAL_FT_PIECE_SQUARE_TABLE | 
		EVAL_FT_PIECE_MOBILITY | 
		EVAL_FT_PAWN_STRUCUTRE |
		EVAL_FT_BISHOP_PAIR;

	// Alle Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_ALL = 0xff;

	// Faktoren (Mit welcher Gewichtung die Features Einfluss auf die Bewertung haben 0.0 = 0% und 1.0 = 100%)

	constexpr float MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT	= 1.00f; // Materialwerte zu Spielphase (Faktor)
	constexpr float PIECE_SQUARE_TABLE_WEIGHT			= 1.00f; // Piece-Square-Tabelle (Faktor)
	constexpr float PIECE_MOBILITY_WEIGHT				= 1.00f; // Piece-Mobility (Faktor)
	constexpr float BISHOP_PAIR_BONUS_WEIGHT			= 1.00f; // Bauernstruktur (Faktor)
	constexpr float PAWN_STRUCTURE_WEIGHT				= 1.00f; // Läuferpaar (Faktor)

	// Spielphase Bedingungen
	constexpr unsigned short MIDGAME_NUMBER = 12;			// Die Zugnummer, ab der die Partie als Mittelspiel betrachtet wird
	constexpr float MINIMUM_BALANCE_FOR_ENDGAME = 17.00f;	// Das Minimum des einfachen gesamten Materialwertes (W+S, ohne Bauern), ab der die Partie als Endspiel betrachtet wird

	// Spielphasen:
	constexpr unsigned short GAME_PHASE_START	= 0;	// Eröffnungsphase
	constexpr unsigned short GAME_PHASE_MID		= 1;	// Mittelspielphase
	constexpr unsigned short GAME_PHASE_END		= 2;	// Endspielphase

	// Materialwert + Addition für die jeweilige Spielphase				//  P	   N	  B		 R		Q
	constexpr std::array<float, 5> MATERIAL_VALUES						= { 1.00f, 3.00f, 3.00f, 5.00f, 9.00f }; // Materialwert
	constexpr std::array<float, 5> MATERIAL_ADDITION_BEGIN_GAME_PHASE	= { 0.00f, 0.25f, 0.25f, 0.00f, 0.00f }; // Materialwert-Addition in der Eröffnung
	constexpr std::array<float, 5> MATERIAL_ADDITION_MID_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.50f, 0.50f }; // Materialwert-Addition im Mittelspiel
	constexpr std::array<float, 5> MATERIAL_ADDITION_END_GAME_PHASE		= { 0.00f, 0.50f, 0.50f, 0.75f, 0.75f }; // Materialwert-Addition im Endspiel

	// Zuordung bzw. Index für MATERIAL_VALUES und weitere:
	constexpr unsigned short PAWN	= 0;	// Bauer-Index
	constexpr unsigned short KNIGHT = 1;	// Springer-Index
	constexpr unsigned short BISHOP = 2;	// Läufer-Index
	constexpr unsigned short ROOK	= 3;	// Turm-Index
	constexpr unsigned short QUEEN	= 4;	// Dame-Index
	constexpr unsigned short KING	= 5;	// König-Index

	constexpr unsigned char PAWN_WHITE = 'P';
	constexpr unsigned char PAWN_BLACK = 'p';
	constexpr unsigned char BISHOP_WHITE = 'B';
	constexpr unsigned char BISHOP_BLACK = 'b';
	constexpr unsigned char KNIGHT_WHITE = 'N';
	constexpr unsigned char KNIGHT_BLACK = 'n';
	constexpr unsigned char ROOK_WHITE = 'R';
	constexpr unsigned char ROOK_BLACK = 'r';
	constexpr unsigned char QUEEN_WHITE = 'Q';
	constexpr unsigned char QUEEN_BLACK = 'q';
	constexpr unsigned char KING_WHITE = 'K';
	constexpr unsigned char KING_BLACK = 'k';

	constexpr unsigned short MAX_PAWN_COUNT = 8;
	constexpr unsigned short MAX_PIECE_TYPES = 6;

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
	constexpr std::array<float, MAX_PAWN_COUNT> MATERIAL_DYNAMIC_PAWNS = { 1.05f, 1.03f, 1.01f, 1.00f, 0.99f, 0.98f, 0.97f, 0.95f };

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

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_PAWN = {
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
		0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f, 0.50f,
		0.10f, 0.10f, 0.20f, 0.30f, 0.30f, 0.20f, 0.10f, 0.10f,
		0.05f, 0.05f, 0.10f, 0.25f, 0.25f, 0.10f, 0.05f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.20f, 0.20f, 0.00f, 0.00f, 0.00f,
		0.05f,-0.05f,-0.10f, 0.00f, 0.00f,-0.10f,-0.05f, 0.05f,
		0.05f, 0.10f, 0.10f,-0.20f,-0.20f, 0.10f, 0.10f, 0.05f,
		0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_KNIGHT = {
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f,
		-0.00f,-0.20f, 0.00f, 0.00f, 0.00f, 0.00f,-0.20f,-0.40f,
		-0.00f, 0.00f, 0.10f, 0.15f, 0.15f, 0.10f, 0.00f,-0.30f,
		-0.00f, 0.05f, 0.15f, 0.20f, 0.20f, 0.15f, 0.05f,-0.30f,
		-0.00f, 0.00f, 0.15f, 0.20f, 0.20f, 0.15f, 0.00f,-0.30f,
		-0.00f, 0.05f, 0.10f, 0.15f, 0.15f, 0.10f, 0.05f,-0.30f,
		-0.40f,-0.20f, 0.00f, 0.05f, 0.05f, 0.00f,-0.20f,-0.40f,
		-0.50f,-0.40f,-0.30f,-0.30f,-0.30f,-0.30f,-0.40f,-0.50f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_BISHOP = {
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f, 0.10f, 0.10f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.05f, 0.05f, 0.10f, 0.10f, 0.05f, 0.05f,-0.10f,
		-0.10f, 0.00f, 0.10f, 0.10f, 0.10f, 0.10f, 0.00f,-0.10f,
		-0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,-0.10f,
		-0.10f, 0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.05f,-0.10f,
		-0.20f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.10f,-0.20f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_ROOK = {
		 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,  0.00f,
		 0.05f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f, 0.10f,  0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		-0.05f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, -0.05f,
		 0.00f, 0.00f, 0.00f, 0.05f, 0.05f, 0.00f, 0.00f,  0.00f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_QUEEN = {
		-0.20f,-0.10f,-0.10f, -0.05, -0.05f,-0.10f,-0.10f,-0.20f,
		-0.10f, 0.00f, 0.00f,  0.00,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.05f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.05f,
		 0.00f, 0.00f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.05f,
		-0.10f, 0.05f, 0.05f,  0.05,  0.05f, 0.05f, 0.00f,-0.10f,
		-0.10f, 0.00f, 0.05f,  0.00,  0.00f, 0.00f, 0.00f,-0.10f,
		-0.20f,-0.10f,-0.10f, -0.05, -0.05f,-0.10f,-0.10f,-0.20f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_KING_MID_GAME = {
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.30f,-0.40f,-0.40f,-0.50f,-0.50f,-0.40f,-0.40f,-0.30f,
		-0.20f,-0.30f,-0.30f,-0.40f,-0.40f,-0.30f,-0.30f,-0.20f,
		-0.10f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.20f,-0.10f,
		 0.20f, 0.20f, 0.00f, 0.00f, 0.00f, 0.00f, 0.20f, 0.20f,
		 0.20f, 0.30f, 0.10f, 0.00f, 0.00f, 0.10f, 0.30f, 0.20f
	};

	constexpr std::array<float, 64> PIECE_SQUARE_TABLE_KING_END_GAME = {
		-0.50f,-0.40f,-0.30f,-0.20f,-0.20f,-0.30f,-0.40f,-0.50f,
		-0.30f,-0.20f,-0.10f, 0.00f, 0.00f,-0.10f,-0.20f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.30f, 0.40f, 0.40f, 0.30f,-0.10f,-0.30f,
		-0.30f,-0.10f, 0.20f, 0.30f, 0.30f, 0.20f,-0.10f,-0.30f,
		-0.30f,-0.30f, 0.00f, 0.00f, 0.00f, 0.00f,-0.30f,-0.30f,
		-0.50f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.30f,-0.50f
	};

	constexpr std::array<float, 64> MIRROR_PIECE_SQUARE_TABLE(const std::array<float, 64> t)
	{
		std::array<float, 64>&& sort_table =
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
		/// Isolierte Bauern?
		static bool isIsolatedPawn(const Position& position, int x, int y);
		/// Backwards Bauern?
		// TODO: Implement
		static bool isBackwardsPawn(const Position& position, int x, int y);
		/// Passierte Bauern?
		static bool isPassedPawn(const Position& position, int x, int y);
		/// Verkettete Bauern? 
		static bool isChainPawn(const Position& position, int x, int y);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein freundlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static bool isPieceEqualOnOffset(const Position& position, int x, int y, int xOffset, int yOffset);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein feindlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static bool isPieceEnemyPawnOnOffset(const Position& position, int x, int y, int xOffset, int yOffset);
	};
}