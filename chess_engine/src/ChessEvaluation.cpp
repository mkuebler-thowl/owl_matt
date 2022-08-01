#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include <unordered_map>

namespace matt
{
	float ChessEvaluation::evaluate(const Position& position, short enginePlayer, unsigned char evaluationFeatureFlags)
	{
		// Ist die Position eine Endstellung?
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_WHITE)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_BLACK) return -INF;
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_BLACK)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_WHITE) return INF;
		if (position.getGameState() == GameState::Remis) return 0.00f;

		auto white_score = 0.00f;
		auto black_score = 0.00f;

		// Zur Bestimmung des einfachen Materials für Endspielstellung werden Bauernwerte erst zum Schluss addiert
		auto white_pawn_score = 0.00f;
		auto black_pawn_score = 0.00f;

		// Zur Bestimmung der Peace Square Tables:
		auto white_square_table_score = 0.0f;
		auto black_square_table_score = 0.0f;
		std::pair<int, int> white_king_pos = { 0.0f, 0.0f };
		std::pair<int, int> black_king_pos = { 0.0f, 0.0f };

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

		// Datenstruktur für Piece Mobility
		std::unordered_map<char, int> possible_moves;

		possible_moves[PAWN_WHITE] = 0;
		possible_moves[PAWN_BLACK] = 0;
		possible_moves[KNIGHT_WHITE] = 0;
		possible_moves[KNIGHT_BLACK] = 0;
		possible_moves[BISHOP_WHITE] = 0;
		possible_moves[BISHOP_BLACK] = 0;
		possible_moves[ROOK_WHITE] = 0;
		possible_moves[ROOK_BLACK] = 0;
		possible_moves[QUEEN_WHITE] = 0;
		possible_moves[QUEEN_BLACK] = 0;
		possible_moves[KING_WHITE] = 0;
		possible_moves[KING_BLACK] = 0;

		// Pro Spielfeld Figure zählen und Materialwert hinzutragen
		// Anmerkung: Der Materialwert der Bauern wird später zu white_score bzw. black_score hinzugetragen, 
		// da der Wert für die Berechnung, ob die Position eine Endstellung ist, nicht berücksichtig werden soll
		for (int y = 0; y < ROWS; y++)
		{
			for (int x = 0; x < COLUMNS; x++)
			{
				switch (position[y][x])
				{
				case PAWN_WHITE:
					white_pawns++;
					// Bauernstruktur für Weiß bestimmen
					if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCUTRE)
					{
						if (isDoublePawn(position, x, y))		white_pawn_score += PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
						if (isConnectedPawn(position, x, y))	white_pawn_score += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isChainPawn(position, x, y))		white_pawn_score += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isIsolatedPawn(position, x, y))		white_pawn_score += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
						if (isPassedPawn(position, x, y))		white_pawn_score += PAWN_STRUCTURE_PASSED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isBackwardsPawn(position, x, y))	white_pawn_score += PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					}
					break;
				case PAWN_BLACK:
					black_pawns++;
					// Bauernstruktur für Schwarz bestimmen
					if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCUTRE)
					{
						if (isDoublePawn(position, x, y))		black_pawn_score += PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
						if (isConnectedPawn(position, x, y))	black_pawn_score += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isChainPawn(position, x, y))		black_pawn_score += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isIsolatedPawn(position, x, y))		black_pawn_score += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
						if (isPassedPawn(position, x, y))		black_pawn_score += PAWN_STRUCTURE_PASSED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
						if (isBackwardsPawn(position, x, y))	black_pawn_score += PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					}
					break;
				case KNIGHT_WHITE:
					white_knights++; 
					white_score += MATERIAL_VALUES[KNIGHT];
					break;
				case KNIGHT_BLACK:
					black_knights++;
					black_score += MATERIAL_VALUES[KNIGHT];
					break;
				case BISHOP_WHITE:
					white_bishops++;
					white_score += MATERIAL_VALUES[BISHOP];
					break;
				case BISHOP_BLACK:
					black_bishops++;
					black_score += MATERIAL_VALUES[BISHOP];
					break;
				case ROOK_WHITE:
					white_rooks++;
					white_score += MATERIAL_VALUES[ROOK];
					break;
				case ROOK_BLACK:
					black_rooks++;
					black_score += MATERIAL_VALUES[ROOK];
					break;
				case QUEEN_WHITE:
					white_queens++;
					white_score += MATERIAL_VALUES[QUEEN];
					break;
				case QUEEN_BLACK:
					black_queens++;
					black_score += MATERIAL_VALUES[QUEEN];
					break;
				}
				// Piece Mobility Züge zählen
				if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
				{
					possible_moves[position[y][x]] += ChessValidation::countPossibleMovesOnField(position, x, y);
				}

				// Square Table addieren (Zunächst ohne König)
				if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE)
				{
					auto table_index = x + (COLUMNS * y);

					switch (position[y][x])
					{
					case PAWN_WHITE: white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_PAWN_WEIGHT * PIECE_SQUARE_TABLE_PAWN[table_index]; break;
					case PAWN_BLACK: black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_PAWN_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_PAWN)[table_index]; break;
					case KNIGHT_WHITE: white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT[table_index]; break;
					case KNIGHT_BLACK: black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KNIGHT)[table_index]; break;
					case BISHOP_WHITE: white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_BISHOP_WEIGHT * PIECE_SQUARE_TABLE_BISHOP[table_index]; break;
					case BISHOP_BLACK: black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_BISHOP_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_BISHOP)[table_index]; break;
					case ROOK_WHITE: white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_ROOK_WEIGHT * PIECE_SQUARE_TABLE_ROOK[table_index]; break;
					case ROOK_BLACK: black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_ROOK_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_ROOK)[table_index]; break;
					case QUEEN_WHITE: white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_QUEEN_WEIGHT * PIECE_SQUARE_TABLE_QUEEN[table_index]; break;
					case QUEEN_BLACK: black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_QUEEN_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_QUEEN)[table_index]; break;
					// Bestimme die Positionen der Könige, um diese nach den terationen der Square-Table hinzuzufügen, 
					// da die Spielphase erst später bestimmt werden kann und es für Mittel- und Endspiel unterschiedliche Tabellen gibt
					case KING_WHITE: white_king_pos = {x,y};  break;
					case KING_BLACK: black_king_pos = {x,y}; break;
					}
				}
			}
		}

		// Dynamischer Bonus für den Materialwert je Spielphase
		if (evaluationFeatureFlags & EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE)
		{
			// Spielphase bestimmen
			auto game_phase = position.getMoveNumber() >= MIDGAME_NUMBER ? GAME_PHASE_MID : GAME_PHASE_START;
			game_phase = game_phase == GAME_PHASE_MID && white_score + black_score <= MINIMUM_BALANCE_FOR_ENDGAME ? GAME_PHASE_END : game_phase;
			switch (game_phase)
			{
			case GAME_PHASE_START:
				white_score += white_pawns	 * MATERIAL_ADDITION_BEGIN_GAME_PHASE[PAWN]		* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_knights * MATERIAL_ADDITION_BEGIN_GAME_PHASE[KNIGHT]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_bishops * MATERIAL_ADDITION_BEGIN_GAME_PHASE[BISHOP]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_rooks	 * MATERIAL_ADDITION_BEGIN_GAME_PHASE[ROOK]		* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_queens  * MATERIAL_ADDITION_BEGIN_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;

				black_score += black_pawns   * MATERIAL_ADDITION_BEGIN_GAME_PHASE[PAWN]		* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_knights * MATERIAL_ADDITION_BEGIN_GAME_PHASE[KNIGHT]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_bishops * MATERIAL_ADDITION_BEGIN_GAME_PHASE[BISHOP]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_rooks   * MATERIAL_ADDITION_BEGIN_GAME_PHASE[ROOK]		* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_queens  * MATERIAL_ADDITION_BEGIN_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				break;
			case GAME_PHASE_MID:
				white_score += white_pawns	 * MATERIAL_ADDITION_MID_GAME_PHASE[PAWN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_knights * MATERIAL_ADDITION_MID_GAME_PHASE[KNIGHT] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_bishops * MATERIAL_ADDITION_MID_GAME_PHASE[BISHOP] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_rooks	 * MATERIAL_ADDITION_MID_GAME_PHASE[ROOK]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_queens  * MATERIAL_ADDITION_MID_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;

				black_score += black_pawns   * MATERIAL_ADDITION_MID_GAME_PHASE[PAWN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_knights * MATERIAL_ADDITION_MID_GAME_PHASE[KNIGHT] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_bishops * MATERIAL_ADDITION_MID_GAME_PHASE[BISHOP] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_rooks   * MATERIAL_ADDITION_MID_GAME_PHASE[ROOK]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_queens  * MATERIAL_ADDITION_MID_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;

				// Piece Square Table für König anlegen
				if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE)
				{
					auto table_white_index = white_king_pos.first + (COLUMNS * white_king_pos.second);
					auto table_black_index = white_king_pos.first + (COLUMNS * white_king_pos.second);

					white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_MID_GAME[table_white_index];
					black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KING_MID_GAME)[table_black_index];
				}
				break;
			case GAME_PHASE_END:
				white_score += white_pawns	 * MATERIAL_ADDITION_END_GAME_PHASE[PAWN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_knights * MATERIAL_ADDITION_END_GAME_PHASE[KNIGHT] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_bishops * MATERIAL_ADDITION_END_GAME_PHASE[BISHOP] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_rooks	 * MATERIAL_ADDITION_END_GAME_PHASE[ROOK]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				white_score += white_queens  * MATERIAL_ADDITION_END_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;

				black_score += black_pawns	 * MATERIAL_ADDITION_END_GAME_PHASE[PAWN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_knights * MATERIAL_ADDITION_END_GAME_PHASE[KNIGHT] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_bishops * MATERIAL_ADDITION_END_GAME_PHASE[BISHOP] * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_rooks	 * MATERIAL_ADDITION_END_GAME_PHASE[ROOK]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				black_score += black_queens  * MATERIAL_ADDITION_END_GAME_PHASE[QUEEN]	* MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;

				// Piece Square Table für König anlegen
				if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE)
				{
					auto table_white_index = white_king_pos.first + (COLUMNS * white_king_pos.second);
					auto table_black_index = white_king_pos.first + (COLUMNS * white_king_pos.second);

					white_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_END_GAME[table_white_index];
					black_square_table_score += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KING_END_GAME)[table_black_index];
				}
				break;
			}
		}

		// Dynamische Bauern aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_DYNAMIC_PAWNS)
		{
			// Bauernanzahl clampen:
			if (white_pawns > MAX_PAWN_COUNT) white_pawns = MAX_PAWN_COUNT;
			if (black_pawns > MAX_PAWN_COUNT) black_pawns = MAX_PAWN_COUNT;

			// Index Grenzen auf 0-7 festlegen
			auto white_index = white_pawns;
			auto black_index = black_pawns;
			if (white_pawns > 0) white_index--;
			if (black_pawns > 0) black_index--;

			// Dynamische Bauern addieren
			white_pawn_score += white_pawns * MATERIAL_DYNAMIC_PAWNS[white_index];
			black_pawn_score += black_pawns * MATERIAL_DYNAMIC_PAWNS[black_index];
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
			if (white_bishops >= 2) white_score += BISHOP_PAIR_BONUS * BISHOP_PAIR_BONUS_WEIGHT;
			if (black_bishops >= 2) black_score += BISHOP_PAIR_BONUS * BISHOP_PAIR_BONUS_WEIGHT;
		}

		// Piece Mobility hinzufügen:
		if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
		{
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_PAWN_WEIGHT	* possible_moves[PAWN_WHITE];
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_KNIGHT_WEIGHT * possible_moves[KNIGHT_WHITE];
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_BISHOP_WEIGHT * possible_moves[BISHOP_WHITE];
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_ROOK_WEIGHT	* possible_moves[ROOK_WHITE];
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_QUEEN_WEIGHT	* possible_moves[QUEEN_WHITE];
			white_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_KING_WEIGHT	* possible_moves[KING_WHITE];

			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_PAWN_WEIGHT	* possible_moves[PAWN_BLACK];
			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_KNIGHT_WEIGHT * possible_moves[KNIGHT_BLACK];
			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_BISHOP_WEIGHT * possible_moves[BISHOP_BLACK];
			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_ROOK_WEIGHT	* possible_moves[ROOK_BLACK];
			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_QUEEN_WEIGHT	* possible_moves[QUEEN_BLACK];
			black_score += PIECE_MOBILITY_WEIGHT * PIECE_MOBILITY_KING_WEIGHT	* possible_moves[KING_BLACK];
		}

		// Bauern Materialwert addieren
		white_score += white_pawn_score + white_square_table_score;
		black_score += black_pawn_score + black_square_table_score;

		// Je nach Spieler score der Spieler voneinander abziehen und zurückgeben
		return enginePlayer == PLAYER_WHITE ? white_score - black_score : black_score - white_score;
	}
	float ChessEvaluation::getMaterialValueByPiece(char piece)
	{
		switch (piece)
		{
		case PAWN_WHITE:
		case PAWN_BLACK:
			return MATERIAL_VALUES[PAWN];
			break;
		case KNIGHT_WHITE:
		case KNIGHT_BLACK:
			return MATERIAL_VALUES[KNIGHT];
			break;
		case BISHOP_WHITE:
		case BISHOP_BLACK:
			return MATERIAL_VALUES[BISHOP];
			break;
		case ROOK_WHITE:
		case ROOK_BLACK:
			return MATERIAL_VALUES[ROOK];
			break;
		case QUEEN_WHITE:
		case QUEEN_BLACK:
			return MATERIAL_VALUES[QUEEN];
			break;
		}

		return 0.00f;
	}
	bool ChessEvaluation::isDoublePawn(const Position& position, int x, int y)
	{
		if (isPieceEqualOnOffset(position, x, y, 0, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, 0, -1)) return true;

		return false;
	}
	bool ChessEvaluation::isConnectedPawn(const Position& position, int x, int y)
	{
		if (isPieceEqualOnOffset(position, x, y, 1, 0)) return true;		
		if (isPieceEqualOnOffset(position, x, y, -1, 0)) return true;

		return false;
	}
	bool ChessEvaluation::isIsolatedPawn(const Position& position, int x, int y)
	{
		if (isPieceEqualOnOffset(position, x, y, 1, 0)) return false;
		if (isPieceEqualOnOffset(position, x, y, -1, 0)) return false;
		if (isDoublePawn(position, x, y)) return false;
		if (isChainPawn(position, x, y)) return false;

		return true;
	}
	bool ChessEvaluation::isBackwardsPawn(const Position& position, int x, int y)
	{
		auto enemy_pawn = position.getPlayer() == PLAYER_WHITE ? PAWN_BLACK : PAWN_WHITE;
		auto dir = enemy_pawn == PAWN_WHITE ? 1 : -1;

		if (!isConnectedPawn(position, x, y))
		{
			// Gegnerischer Bauer vorne
			if (isPieceEnemyPawnOnOffset(position, x, y, 0, dir)) return true;
			if (isPieceEnemyPawnOnOffset(position, x, y, 1, 2 * dir)) return true;
			if (isPieceEnemyPawnOnOffset(position, x, y, -1, 2 * dir)) return true;
		}

		return false;
	}
	bool ChessEvaluation::isPassedPawn(const Position& position, int x, int y)
	{
		auto enemy_pawn = position[y][x] == PAWN_WHITE ? PAWN_BLACK : PAWN_WHITE;

		for (int i = 0; i < ROWS; i++)
		{
			if (i == y) continue; // Eigenes Feld ignorieren
			if (ChessValidation::isInsideChessboard(x, i))
			{
				if (position[i][x] == enemy_pawn) return false;
			}
		}

		return true;
	}
	bool ChessEvaluation::isChainPawn(const Position& position, int x, int y)
	{
		if (isPieceEqualOnOffset(position, x, y, 1, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, 1, -1)) return true;
		if (isPieceEqualOnOffset(position, x, y, -1, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, -1, -1)) return true;

		return false;
	}

	bool ChessEvaluation::isPieceEqualOnOffset(const Position& position, int x, int y, int xOffset, int yOffset)
	{
		if (ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
	bool ChessEvaluation::isPieceEnemyPawnOnOffset(const Position& position, int x, int y, int xOffset, int yOffset)
	{
		auto startPawn = position.getPlayer() == PLAYER_WHITE ? PAWN_WHITE : PAWN_BLACK;
		auto targetPawn = startPawn == PAWN_WHITE ? PAWN_BLACK : PAWN_WHITE;

		if (ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == startPawn && targetPawn == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
}