#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"
#include "ChessEngine.hpp"

namespace matt
{
	float ChessEvaluation::evaluate(const Position& position, short enginePlayer, unsigned char evaluationFeatureFlags)
	{
		auto white_score = 0.00f;
		auto black_score = 0.00f;

		// Zur Bestimmung des einfachen Materials für Endspielstellung werden Bauernwerte erst zum Schluss addiert
		auto white_pawn_score = 0.00f;
		auto black_pawn_score = 0.00f;

		// Anzahl der Figuren
		unsigned short white_pawns = 0;
		unsigned short black_pawns = 0;
		unsigned short white_knights = 0;
		unsigned short black_knights = 0;
		unsigned short white_bishops = 0;
		unsigned short black_bishops = 0;
		unsigned short white_rooks = 0;
		unsigned short black_rooks = 0;
		unsigned short white_queens = 0;
		unsigned short black_queens = 0;


		auto game_phase = position.getMoveNumber() >= MIDGAME_NUMBER ? GAME_PHASE_MID : GAME_PHASE_START;

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				switch (position[y][x])
				{
				case 'P':
					white_pawns++;
					if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCUTRE)
					{
						if (isDoublePawn(position, x, y)) white_pawn_score += PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY;
						if (isConnectedPawn(position, x, y)) white_pawn_score += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS;
						if (isChainPawn(position, x, y)) white_pawn_score += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS;
						if (isIsolatedPawn(position, x, y)) white_pawn_score += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY;
						if (isPassedPawn(position, x, y)) white_pawn_score += PAWN_STRUCTURE_PASSED_PAWNS_BONUS;
					}
					break;
				case 'p':
					black_pawns++;
					if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCUTRE)
					{
						if (isDoublePawn(position, x, y)) black_pawn_score += PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY;
						if (isConnectedPawn(position, x, y)) black_pawn_score += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS;
						if (isChainPawn(position, x, y)) black_pawn_score += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS;
						if (isIsolatedPawn(position, x, y)) black_pawn_score += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY;
						if (isPassedPawn(position, x, y)) black_pawn_score += PAWN_STRUCTURE_PASSED_PAWNS_BONUS;
					}
					break;
				case 'N':
					white_knights++;
					white_score += MATERIAL_VALUES[KNIGHT];
					break;
				case 'n':
					black_knights++;
					black_score += MATERIAL_VALUES[KNIGHT];
					break;
				case 'B':
					white_bishops++;
					white_score += MATERIAL_VALUES[BISHOP];
					break;
				case 'b':
					black_bishops++;
					black_score += MATERIAL_VALUES[BISHOP];
					break;
				case 'R':
					white_rooks++;
					white_score += MATERIAL_VALUES[ROOK];
					break;
				case 'r':
					black_rooks++;
					black_score += MATERIAL_VALUES[ROOK];
					break;
				case 'Q':
					white_queens++;
					white_score += MATERIAL_VALUES[QUEEN];
					break;
				case 'q':
					black_queens++;
					black_score += MATERIAL_VALUES[QUEEN];
					break;
				}
			}
		}

		if (game_phase == GAME_PHASE_MID && white_score + black_score <= MINIMUM_BALANCE_FOR_ENDGAME)
		{
			game_phase = GAME_PHASE_END;
		}

		switch (game_phase)
		{
		case GAME_PHASE_START:
			white_score += white_pawns		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[PAWN];
			white_score += white_knights	* MATERIAL_ADDITION_BEGIN_GAME_PHASE[KNIGHT];
			white_score += white_bishops	* MATERIAL_ADDITION_BEGIN_GAME_PHASE[BISHOP];
			white_score += white_rooks		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[ROOK];
			white_score += white_queens		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[QUEEN];
			
			black_score += black_pawns		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[PAWN];
			black_score += black_knights	* MATERIAL_ADDITION_BEGIN_GAME_PHASE[KNIGHT];
			black_score += black_bishops	* MATERIAL_ADDITION_BEGIN_GAME_PHASE[BISHOP];
			black_score += black_rooks		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[ROOK];
			black_score += black_queens		* MATERIAL_ADDITION_BEGIN_GAME_PHASE[QUEEN];
			break;
		case GAME_PHASE_MID:
			white_score += white_pawns		* MATERIAL_ADDITION_MID_GAME_PHASE[PAWN];
			white_score += white_knights	* MATERIAL_ADDITION_MID_GAME_PHASE[KNIGHT];
			white_score += white_bishops	* MATERIAL_ADDITION_MID_GAME_PHASE[BISHOP];
			white_score += white_rooks		* MATERIAL_ADDITION_MID_GAME_PHASE[ROOK];
			white_score += white_queens		* MATERIAL_ADDITION_MID_GAME_PHASE[QUEEN];
													
			black_score += black_pawns		* MATERIAL_ADDITION_MID_GAME_PHASE[PAWN];
			black_score += black_knights	* MATERIAL_ADDITION_MID_GAME_PHASE[KNIGHT];
			black_score += black_bishops	* MATERIAL_ADDITION_MID_GAME_PHASE[BISHOP];
			black_score += black_rooks		* MATERIAL_ADDITION_MID_GAME_PHASE[ROOK];
			black_score += black_queens		* MATERIAL_ADDITION_MID_GAME_PHASE[QUEEN];
			break;
		case GAME_PHASE_END:
			white_score += white_pawns		* MATERIAL_ADDITION_END_GAME_PHASE[PAWN];
			white_score += white_knights	* MATERIAL_ADDITION_END_GAME_PHASE[KNIGHT];
			white_score += white_bishops	* MATERIAL_ADDITION_END_GAME_PHASE[BISHOP];
			white_score += white_rooks		* MATERIAL_ADDITION_END_GAME_PHASE[ROOK];
			white_score += white_queens		* MATERIAL_ADDITION_END_GAME_PHASE[QUEEN];

			black_score += black_pawns		* MATERIAL_ADDITION_END_GAME_PHASE[PAWN];
			black_score += black_knights	* MATERIAL_ADDITION_END_GAME_PHASE[KNIGHT];
			black_score += black_bishops	* MATERIAL_ADDITION_END_GAME_PHASE[BISHOP];
			black_score += black_rooks		* MATERIAL_ADDITION_END_GAME_PHASE[ROOK];
			black_score += black_queens		* MATERIAL_ADDITION_END_GAME_PHASE[QUEEN];
			break;
		}

		// Dynamische Bauern aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_DYNAMIC_PAWNS)
		{
			// Bauernanzahl clampen:
			if (white_pawns > 8) white_pawns = 8;
			if (black_pawns > 8) black_pawns = 8;

			// Dynamische Bauern addieren
			white_pawn_score += white_pawns * MATERIAL_DYNAMIC_PAWNS[white_pawns];
			black_pawn_score += black_pawns * MATERIAL_DYNAMIC_PAWNS[black_pawns];
		}
		// Ansonsten normale Bauerneinheit addieren
		else
		{
			white_pawn_score += white_pawns * MATERIAL_VALUES[PAWN];
			black_pawn_score += black_pawns * MATERIAL_VALUES[PAWN];
		}

		// Läuferpaar aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_BISHOP_PAIR)
		{
			if (white_bishops >= 2) white_score += BISHOP_PAIR_BONUS;
			if (black_bishops >= 2) black_score += BISHOP_PAIR_BONUS;
		}

		white_score += white_pawn_score;
		black_score += black_pawn_score;

		return enginePlayer == PLAYER_WHITE ? white_score - black_score : black_score - white_score;
	}
	bool ChessEvaluation::isDoublePawn(const Position& position, int x, int y)
	{
		if (isInsideAndPawnStructure(position, x, y, 0, 1)) return true;
		if (isInsideAndPawnStructure(position, x, y, 0, -1)) return true;

		return false;
	}
	bool ChessEvaluation::isConnectedPawn(const Position& position, int x, int y)
	{
		if (isInsideAndPawnStructure(position, x, y, 1, 0)) return true;		
		if (isInsideAndPawnStructure(position, x, y, -1, 0)) return true;

		return false;
	}
	bool ChessEvaluation::isIsolatedPawn(const Position& position, int x, int y)
	{


		return false;
	}
	bool ChessEvaluation::isBackwardsPawn(const Position& position, int x, int y)
	{
		return false;
	}
	bool ChessEvaluation::isPassedPawn(const Position& position, int x, int y)
	{
		return false;
	}
	bool ChessEvaluation::isChainPawn(const Position& position, int x, int y)
	{
		if (isInsideAndPawnStructure(position, x, y, 1, 1)) return true;
		if (isInsideAndPawnStructure(position, x, y, 1, -1)) return true;
		if (isInsideAndPawnStructure(position, x, y, -1, 1)) return true;
		if (isInsideAndPawnStructure(position, x, y, -1, -1)) return true;

		return false;
	}

	bool ChessEvaluation::isInsideAndPawnStructure(const Position& position, int x, int y, int xOffset, int yOffset)
	{
		if (!ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
}