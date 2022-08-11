#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include <unordered_map>

namespace owl
{
	float ChessEvaluation::evaluate(const Position& position, short enginePlayer, unsigned char evaluationFeatureFlags)
	{
		// Ist die Position eine Endstellung?
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_WHITE)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_BLACK) return -INF;
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_BLACK)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_WHITE) return INF;
		if (position.getGameState() == GameState::Remis) return 0.00f;

		float score[PLAYER_COUNT] = { 0.0f };					// Score
		float extra_pawn_score[PLAYER_COUNT] = { 0.0f };		// Bauernscore (Wird nachträglich zum Score addiert)
		float square_table_score[PLAYER_COUNT] = { 0.0f };		// Square-Table Score

		std::pair<int, int> king_pos[PLAYER_COUNT] = {{0, 0}};

		unsigned short piece_count[PLAYER_COUNT][MAX_PIECE_TYPES] = { {0} };	 // Figurenanzahl
		unsigned short possible_moves[PLAYER_COUNT][MAX_PIECE_TYPES] = { {0} }; // Piece Mobility Count

		// Pro Spielfeld Figure zählen und Materialwert hinzutragen
		// Anmerkung: Der Materialwert der Bauern wird später zu white_score bzw. black_score hinzugetragen, 
		// da der Wert für die Berechnung, ob die Position eine Endstellung ist, nicht berücksichtig werden soll
		for (int y = FIRSTROWINDEX; y < ROWS; y++)
		{
			for (int x = FIRSTCOLINDEX; x < COLUMNS; x++)
			{
				auto piece = position[y][x];

				// Leere Felder überspringen:
				if (piece == EMPTY_PLACE)
					continue;

				auto type = GET_PIECE_INDEX_BY_TYPE(piece);
				auto color = GET_PIECE_COLOR_BY_TYPE(piece);

				piece_count[color][type]++;

				// Bauernstruktur bestimmen
				if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCTURE && type == PAWN)
				{
					auto is_double = isDoublePawn(position, x, y);
					auto is_connected = isConnectedPawn(position, x, y);
					auto is_chain = isChainPawn(position, x, y);
					auto is_passed = isPassedPawn(position, x, y);

					auto is_isolated = !(is_double && is_connected && is_chain);
					auto is_backwards = !is_connected ? isBackwardsPawn(position, x, y) : false;

					if (is_double)		extra_pawn_score[color] += PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					if (is_connected)	extra_pawn_score[color] += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
					if (is_chain)		extra_pawn_score[color] += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
					if (is_passed)		extra_pawn_score[color] += PAWN_STRUCTURE_PASSED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
					
					if (is_isolated)	extra_pawn_score[color] += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					if (is_backwards)	extra_pawn_score[color] += PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
				}

				score[color] += MATERIAL_VALUES[type];

				// Piece Mobility Züge zählen
				if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
				{
					possible_moves[color][type] += ChessValidation::countPossibleMovesOnField(position, x, y);
				}

				// Square Table addieren (Zunächst ohne König)
				if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE)
				{
					auto table_index = x + (COLUMNS * y);
					auto amount = 0.0f;

					switch (type)
					{
					case PAWN:		amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_PAWN_WEIGHT	* PIECE_SQUARE_TABLE_PAWN[table_index]; break;
					case KNIGHT:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT_WEIGHT	* PIECE_SQUARE_TABLE_KNIGHT[table_index]; break;
					case BISHOP:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_BISHOP_WEIGHT	* PIECE_SQUARE_TABLE_BISHOP[table_index]; break;
					case ROOK:		amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_ROOK_WEIGHT	* PIECE_SQUARE_TABLE_ROOK[table_index]; break;
					case QUEEN:		amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_QUEEN_WEIGHT	* PIECE_SQUARE_TABLE_QUEEN[table_index]; break;
					// Bestimme die Positionen der Könige, um diese nach den Iterationen der Square-Table hinzuzufügen, 
					// da die Spielphase erst später bestimmt werden kann und es für Mittel- und Endspiel unterschiedliche Tabellen gibt
					case KING: king_pos[color] = { x,y }; break;
					}
					square_table_score[color] += amount;
				}
			}
		}

		// Spielphase abfragen
		auto game_phase = position.getGamePhase();
		auto material = score[WHITE] + score[BLACK];

		// Spielphase bestimmen bzw. anpassen
		if (game_phase == GamePhase::Opening && material <= MAX_MATERIAL_SUM_MID_GAME)	position.enterNextGamePhase();
		if (game_phase == GamePhase::Mid && material <= MAX_MATERIAL_SUM_END_GAME)		position.enterNextGamePhase();
		
		// Dynamische Bauern aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_DYNAMIC_PAWNS)
		{
			for (auto color = FIRST_PLAYER_INDEX; color < PLAYER_COUNT; color++)
			{
				auto index = std::min(piece_count[color][PAWN], DYNAMIC_PAWNS_LAST_INDEX); // Grenzen einhalten
				extra_pawn_score[color] += piece_count[color][PAWN] * MATERIAL_DYNAMIC_PAWNS[index];
			}
		}

		// Dynamischer Bonus für den Materialwert je Spielphase
		if (evaluationFeatureFlags & EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE)
		{
			for (int color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				for (int type_index = FIRST_PIECE_TYPES_INDEX; type_index < MAX_PIECE_TYPES; type_index++)
				{
					auto factor = 1.0f;
					switch (game_phase)
					{
					case GamePhase::Opening: factor = MATERIAL_ADDITION_BEGIN_GAME_PHASE[type_index]; break;
					case GamePhase::Mid: factor = MATERIAL_ADDITION_MID_GAME_PHASE[type_index]; break;
					case GamePhase::End: factor = MATERIAL_ADDITION_END_GAME_PHASE[type_index]; break;
					}
					score[color_index] += piece_count[color_index][type_index] * factor * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				}
			}
		}

		// Piece Square Table für König hinzufügen
		if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE && game_phase != GamePhase::Opening)
		{
			for (auto color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				auto table_index = king_pos[color_index].first + ((COLUMNS * king_pos[color_index].second));
				auto factor = 0.0f;
				switch (game_phase)
				{
				case GamePhase::Mid: factor = PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_MID_GAME[table_index]; break;
				case GamePhase::End: factor = PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_END_GAME[table_index]; break;
				}
				square_table_score[color_index] += piece_count[color_index][KING] * factor;
			}
		}

		// Läuferpaar aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_BISHOP_PAIR)
		{
			for (auto color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				if (piece_count[color_index][BISHOP] >= MIN_BISHOP_COUNT_PRECONDITION_BONUS)
					score[color_index] += BISHOP_PAIR_BONUS * BISHOP_PAIR_BONUS_WEIGHT;
			}
		}

		// Piece Mobility hinzufügen:
		if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
		{
			for (int color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				for (int type_index = FIRST_PIECE_TYPES_INDEX; type_index < MAX_PIECE_TYPES; type_index++)
				{
					auto factor = 1.0f;
					switch (type_index)
					{
					case PAWN: factor = PIECE_MOBILITY_PAWN_WEIGHT; break;
					case KNIGHT: factor = PIECE_MOBILITY_KNIGHT_WEIGHT; break;
					case BISHOP: factor = PIECE_MOBILITY_BISHOP_WEIGHT; break;
					case ROOK: factor = PIECE_MOBILITY_ROOK_WEIGHT; break;
					case QUEEN: factor = PIECE_MOBILITY_QUEEN_WEIGHT; break;
					case KING: factor = PIECE_MOBILITY_KING_WEIGHT; break;
					}
					score[color_index] += PIECE_MOBILITY_WEIGHT * factor * possible_moves[color_index][type_index];
				}
			}
		}

		// Materialwerte summieren
		for (int color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
		{
			score[color_index] += extra_pawn_score[color_index] + square_table_score[color_index];
		}

		// Je nach Spieler score der Spieler voneinander abziehen und zurückgeben
		auto&& final_score = enginePlayer == PLAYER_WHITE ?
			score[WHITE] - score[BLACK] : score[BLACK] - score[WHITE];

		return final_score;
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

	bool ChessEvaluation::isBackwardsPawn(const Position& position, int x, int y)
	{
		// Hinweis: isConnectedPawn(...) muss vorher false sein
		auto dir = position.getPlayer();

		// Gegnerischer Bauer vorne
		if (isPieceEnemyPawnOnOffset(position, x, y, 0, dir)) return true;
		if (isPieceEnemyPawnOnOffset(position, x, y, 1, 2 * dir)) return true;
		if (isPieceEnemyPawnOnOffset(position, x, y, -1, 2 * dir)) return true;

		return false;
	}
	bool ChessEvaluation::isPassedPawn(const Position& position, int x, int y)
	{
		auto enemy_pawn = GetEnemyPiece(position.getPlayer(), PAWN);

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
		auto start_color = GetPlayerIndexByPositionPlayer(position.getPlayer());
		auto target_color = start_color + 1 % PLAYER_COUNT;

		auto start_pawn = PIECES[start_color][PAWN];
		auto target_pawn = PIECES[target_color][PAWN];

		if (ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == start_pawn && target_pawn == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
	unsigned char ChessEvaluation::GetPlayerIndexByPositionPlayer(short currentPlayerOfPosition)
	{
		return currentPlayerOfPosition == PLAYER_WHITE ? WHITE : BLACK;
	}
	unsigned char ChessEvaluation::GetEnemyPiece(short currentPlayerOfPosition, unsigned short pieceIndex)
	{
		auto&& enemy_color = GetPlayerIndexByPositionPlayer(-currentPlayerOfPosition);
		return PIECES[enemy_color][pieceIndex];
	}
}