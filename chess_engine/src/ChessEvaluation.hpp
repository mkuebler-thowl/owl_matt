#pragma once

#include <array>
#include "Position.hpp"

namespace matt
{
	// Bewertungsfunktions-Feature
	constexpr unsigned char EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE = (1 << 0); // Materialwerte abhängig von der Spielphase (Eröffnung, Mittel- und Endspiel)
	constexpr unsigned char EVAL_FT_SIMPLE_PEACE_SQUARE			= (1 << 1); // Piece-Square-Tabelle
	constexpr unsigned char EVAL_FT_PIECE_MOBILITY				= (1 << 2); // Piece-Mobility
	constexpr unsigned char EVAL_FT_PAWN_STRUCUTRE				= (1 << 3); // Bauernstruktur (Double, Isolated, Connected, Backwards, Chain, Passed)
	constexpr unsigned char EVAL_FT_BISHOP_PAIR					= (1 << 4); // Läuferpaar
	// optionale/weitere Featuers
	constexpr unsigned char EVAL_FT_DYNAMIC_PAWNS = (1 << 5);	// Dynamische Bauern

	// Alle Standard Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_STANDARD = 
		EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE | 
		EVAL_FT_SIMPLE_PEACE_SQUARE | 
		EVAL_FT_PIECE_MOBILITY | 
		EVAL_FT_PAWN_STRUCUTRE |
		EVAL_FT_BISHOP_PAIR;

	// Alle Bewertungsfunktions-Features
	constexpr unsigned char EVAL_FT_ALL = 0xff;

	// Spielphase Bedingungen
	constexpr unsigned short MIDGAME_NUMBER = 12;			// Die Zugnummer, ab der die Partie als Mittelspiel betrachtet wird
	constexpr float MINIMUM_BALANCE_FOR_ENDGAME = 17.00f;	// Das Minimum des einfachen gesamten Materialwertes (W+S, ohne Bauern), ab der die Partie als Endspiel betrachtet wird

	// Spielphasen:
	constexpr unsigned char GAME_PHASE_START	= 0;	// Eröffnungsphase
	constexpr unsigned char GAME_PHASE_MID		= 1;	// Mittelspielphase
	constexpr unsigned char GAME_PHASE_END		= 2;	// Endspielphase

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

	// Bonus
	constexpr float BISHOP_PAIR_BONUS = 0.50f; // Läuferpaar-Bonus

	// Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_PAWN_FACTOR		= 0.10f; // Bauern-Figurbewegung (Faktor)
	constexpr float PIECE_MOBILITY_KNIGHT_FACTOR	= 0.10f; // Springer-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_BISHOP_FACTOR	= 0.10f; // Läufer-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_ROOK_FACTOR		= 0.10f; // Turm-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_QUEEN_FACTOR		= 0.10f; // Dame-Figurenbewegung (Faktor)
	constexpr float PIECE_MOBILITY_KING_FACTOR		= 0.10f; // König-Figurenbewegung (Faktor)

	// Zusätzliche Dynamische Bauerngewichtung
	constexpr std::array<float, 8> MATERIAL_DYNAMIC_PAWNS = { 0.05f, 0.03f, 0.01f, 0.00f, -0.01f, -0.02f -0.03f, -0.05f };

	// Bauernstruktur Malus
	constexpr float PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY		= -0.200f;	// Doppelte Bauern Malus
	constexpr float PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY	= -0.100f;	// Isolierte Bauern Malus
	constexpr float PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY	= -0.125f;	// Backwards Bauern Malus
	// Bauernstruktur Bonus
	constexpr float PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS	= 0.100f;	// Connected Bauern Bonus
	constexpr float PAWN_STRUCTURE_CHAIN_PAWNS_BONUS		=  0.100f;	// Chain Bauern Bonus
	constexpr float PAWN_STRUCTURE_PASSED_PAWNS_BONUS		=  0.200f;	// Passierte Bauern Bonus

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
		static bool isBackwardsPawn(const Position& position, int x, int y);
		/// Passierte Bauern?
		static bool isPassedPawn(const Position& position, int x, int y);
		/// Verkettete Bauern? 
		static bool isChainPawn(const Position& position, int x, int y);
		/// Hilfsfunktion zur Überprüfung, ob Grenzen eingehalten werden und ob sich ein freundlicher Bauer bei der Position x+xOffset, y+yOffset befindet
		static bool isInsideAndPawnStructure(const Position& position, int x, int y, int xOffset, int yOffset);
	};
}