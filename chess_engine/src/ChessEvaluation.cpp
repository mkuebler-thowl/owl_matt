#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include <unordered_map>

namespace owl
{
	EVALUATION_VALUE ChessEvaluation::evaluate(Position& position, INT32 enginePlayer, UCHAR evaluationFeatureFlags, BOOL validationCheckMate)
	{
		// Checkmate überprüfen?
		// Hinweis: getValidMoves() berechnet das Checkmate intern und es soll nur bei in Schach-Stellungen
		// die Liste der möglichen Züge berechnet werden. Andernfalls wird nicht von einer Endstellung ausgegangen.
		// Vorteil: Mehr Effizienz
		if (validationCheckMate)
		{
			// Gegnerische König im Schach && Gegnerischer Spieler am Zug?
			if (ChessValidation::isKingInCheck(position, -enginePlayer) && position.getPlayer() == -enginePlayer)
			{
				ChessValidation::getValidMoves(position, -enginePlayer);
			}
			// Eigener König im Schach && Selber am Zug?
			else if (ChessValidation::isKingInCheck(position, enginePlayer) && position.getPlayer() == enginePlayer)
			{
				ChessValidation::getValidMoves(position, enginePlayer);
			}
		}

		// Ist die Position eine Endstellung?
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_WHITE)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_BLACK) 
			return -INF;
		if ((position.getGameState() == GameState::PlayerBlackWins && enginePlayer == PLAYER_BLACK)
			|| position.getGameState() == GameState::PlayerWhiteWins && enginePlayer == PLAYER_WHITE) 
			return INF;
		if (position.getGameState() == GameState::Remis) return 0.00f;

	#if OWL_USE_EVALUATION_COMPONENT_REPRESENTAION==true
		EVALUATION_VALUE data[PLAYER_COUNT];

		std::array<FLOAT*, PLAYER_COUNT> score				= {&data[WHITE_INDEX].score,		 &data[BLACK_INDEX].score };
		std::array<FLOAT*, PLAYER_COUNT> material			= {&data[WHITE_INDEX].material,		 &data[BLACK_INDEX].material };
		std::array<FLOAT*, PLAYER_COUNT> game_phase			= {&data[WHITE_INDEX].gamePhase,	 &data[BLACK_INDEX].gamePhase };
		std::array<FLOAT*, PLAYER_COUNT> square_table		= {&data[WHITE_INDEX].squareTable,	 &data[BLACK_INDEX].squareTable };
		std::array<FLOAT*, PLAYER_COUNT> piece_mobility		= {&data[WHITE_INDEX].pieceMobility, &data[BLACK_INDEX].pieceMobility };
		std::array<FLOAT*, PLAYER_COUNT> pawn_structure		= {&data[WHITE_INDEX].pawnStructure, &data[BLACK_INDEX].pawnStructure };
		std::array<FLOAT*, PLAYER_COUNT> bishop_pair		= {&data[WHITE_INDEX].bishopPair,	 &data[BLACK_INDEX].bishopPair };
		std::array<FLOAT*, PLAYER_COUNT> dynamic_pawns		= {&data[WHITE_INDEX].dynamicPawns,	 &data[BLACK_INDEX].dynamicPawns };
	#else
		FLOAT _score[PLAYER_COUNT]			= { 0.0f };
		FLOAT _material[PLAYER_COUNT]		= { 0.0f };
		FLOAT _game_phase[PLAYER_COUNT]		= { 0.0f };
		FLOAT _square_table[PLAYER_COUNT]	= { 0.0f };
		FLOAT _piece_mobility[PLAYER_COUNT]	= { 0.0f };
		FLOAT _pawn_structure[PLAYER_COUNT]	= { 0.0f };
		FLOAT _bishop_pair[PLAYER_COUNT]	= { 0.0f };
		FLOAT _dynamic_pawns[PLAYER_COUNT]	= { 0.0f };

		std::array<FLOAT*, PLAYER_COUNT> score				= {&_score[WHITE_INDEX], &_score[BLACK_INDEX] };
		std::array<FLOAT*, PLAYER_COUNT> material			= {&_material[WHITE_INDEX], &_material[BLACK_INDEX] };
		std::array<FLOAT*, PLAYER_COUNT> game_phase			= {&_game_phase[WHITE_INDEX], &_game_phase[BLACK_INDEX] };
		std::array<FLOAT*, PLAYER_COUNT> square_table		= {&_square_table[WHITE_INDEX],	&_square_table[BLACK_INDEX]};
		std::array<FLOAT*, PLAYER_COUNT> piece_mobility		= {&_piece_mobility[WHITE_INDEX], &_piece_mobility[BLACK_INDEX]};
		std::array<FLOAT*, PLAYER_COUNT> pawn_structure		= {&_pawn_structure[WHITE_INDEX], &_pawn_structure[BLACK_INDEX]};
		std::array<FLOAT*, PLAYER_COUNT> bishop_pair		= {&_bishop_pair[WHITE_INDEX], &_bishop_pair[BLACK_INDEX]};
		std::array<FLOAT*, PLAYER_COUNT> dynamic_pawns		= {&_dynamic_pawns[WHITE_INDEX], &_dynamic_pawns[BLACK_INDEX]};
	#endif

		PAIR<INT32, INT32> king_pos[PLAYER_COUNT] = {{0, 0}};

		INT32 piece_count[PLAYER_COUNT][MAX_PIECE_TYPES] = { {0} };	 // Figurenanzahl
		INT32 possible_moves[PLAYER_COUNT][MAX_PIECE_TYPES] = { {0} }; // Piece Mobility Count

		// Pro Spielfeld Figure zählen und Materialwert hinzutragen
		// Anmerkung: Der Materialwert der Bauern wird später zu white_score bzw. black_score hinzugetragen, 
		// da der Wert für die Berechnung, ob die Position eine Endstellung ist, nicht berücksichtig werden soll
		for (INT32 y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (INT32 x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = position[y][x];

				// Leere Felder überspringen:
				if (piece == EMPTY_FIELD)
					continue;

				auto type = GET_PIECE_INDEX_BY_TYPE(piece);
				auto color = GET_PLAYER_INDEX_BY_PIECE(piece);

				piece_count[color][type]++;

				auto passed_progress_index = color == WHITE_INDEX ? LAST_ROW_INDEX - y : y;

				// Bauernstruktur bestimmen
				if (evaluationFeatureFlags & EVAL_FT_PAWN_STRUCTURE && type == PAWN_INDEX)
				{
					auto is_double = isDoublePawn(position, x, y);
					auto is_connected = isConnectedPawn(position, x, y);
					auto is_chain = isChainPawn(position, x, y);
					auto is_passed = isPassedPawn(position, x, y);

					auto is_isolated = !(is_connected || is_chain);
					auto is_backwards = !is_connected ? isBackwardsPawn(position, x, y) : false;

					auto is_isolated_double = is_double && is_isolated;
					auto is_connected_passed = is_connected && is_passed;

					if (is_double)		*pawn_structure[color] += PAWN_STRUCTURE_DOUBLE_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					if (is_connected)	*pawn_structure[color] += PAWN_STRUCTURE_CONNECTED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
					if (is_chain)		*pawn_structure[color] += PAWN_STRUCTURE_CHAIN_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
					if (is_passed)		*pawn_structure[color] += (PAWN_STRUCTURE_PASSED_PAWNS_BONUS + PASSED_PAWNS_PROGRESS_BONUS[passed_progress_index]) * PAWN_STRUCTURE_WEIGHT;
				
					if (is_isolated)	*pawn_structure[color] += PAWN_STRUCTURE_ISOLATED_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					if (is_backwards)	*pawn_structure[color] += PAWN_STRUCTURE_BACKWARDS_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;

					if (is_isolated_double) *pawn_structure[color] += PAWN_STRUCTURE_ISOLATED_DOUBLE_PAWNS_PENALTY * PAWN_STRUCTURE_WEIGHT;
					if (is_connected_passed) *pawn_structure[color] += PAWN_STRUCTURE_CONNECTED_PASSED_PAWNS_BONUS * PAWN_STRUCTURE_WEIGHT;
				}

				*material[color] += MATERIAL_VALUES[type];

				// Piece Mobility Züge zählen
				if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
				{
					possible_moves[color][type] += ChessValidation::countPossibleMovesOnField(position, x, y, true);
				}

				// Square Table addieren (Zunächst ohne König)
				if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE)
				{
					auto table_index = x + (COLUMNS * y);
					auto amount = 0.0f;

					switch (piece)
					{
					case WHITE_PAWN:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_PAWN_WEIGHT	* PIECE_SQUARE_TABLE_PAWN[table_index]; break;
					case WHITE_KNIGHT:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT_WEIGHT	* PIECE_SQUARE_TABLE_KNIGHT[table_index]; break;
					case WHITE_BISHOP:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_BISHOP_WEIGHT	* PIECE_SQUARE_TABLE_BISHOP[table_index]; break;
					case WHITE_ROOK:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_ROOK_WEIGHT	* PIECE_SQUARE_TABLE_ROOK[table_index]; break;
					case WHITE_QUEEN:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_QUEEN_WEIGHT	* PIECE_SQUARE_TABLE_QUEEN[table_index]; break;

					case BLACK_PAWN:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_PAWN_WEIGHT	* MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_PAWN)[table_index]; break;
					case BLACK_KNIGHT:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_KNIGHT_WEIGHT	* MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KNIGHT)[table_index]; break;
					case BLACK_BISHOP:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_BISHOP_WEIGHT	* MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_BISHOP)[table_index]; break;
					case BLACK_ROOK:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_ROOK_WEIGHT	* MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_ROOK)[table_index]; break;
					case BLACK_QUEEN:	amount += PIECE_SQUARE_TABLE_WEIGHT * PIECE_SQUARE_TABLE_QUEEN_WEIGHT	* MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_QUEEN)[table_index]; break;
					// Bestimme die Positionen der Könige, um diese nach den Iterationen der Square-Table hinzuzufügen, 
					// da die Spielphase erst später bestimmt werden kann und es für Mittel- und Endspiel unterschiedliche Tabellen gibt
					case WHITE_KING:
					case BLACK_KING: 
						king_pos[color] = { x,y }; 
						break;
					}
					*square_table[color] += amount;
				}
			}
		}

		// Spielphase abfragen
		auto pos_game_phase = position.getGamePhase();
		auto both_material = *score[WHITE_INDEX] + *score[BLACK_INDEX];

		// Spielphase bestimmen bzw. anpassen
		if (pos_game_phase == GamePhase::Opening && both_material <= MAX_MATERIAL_SUM_MID_GAME)	position.enterNextGamePhase();
		if (pos_game_phase == GamePhase::Mid && both_material <= MAX_MATERIAL_SUM_END_GAME)		position.enterNextGamePhase();
		
		// Dynamischer Bonus für den Materialwert je Spielphase
		if (evaluationFeatureFlags & EVAL_FT_MATERIAL_DYNAMIC_GAME_PHASE)
		{
			for (INT32 color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				for (INT32 type_index = FIRST_PIECE_TYPES_INDEX; type_index < MAX_PIECE_TYPES; type_index++)
				{
					auto factor = 1.0f;
					switch (pos_game_phase)
					{
					case GamePhase::Opening: factor = MATERIAL_ADDITION_BEGIN_GAME_PHASE[type_index]; break;
					case GamePhase::Mid: factor = MATERIAL_ADDITION_MID_GAME_PHASE[type_index]; break;
					case GamePhase::End: factor = MATERIAL_ADDITION_END_GAME_PHASE[type_index]; break;
					}
					*game_phase[color_index] += piece_count[color_index][type_index] * factor * MATERIAL_DYNAMIC_GAME_PHASE_WEIGHT;
				}
			}
		}

		// Dynamische Bauern aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_DYNAMIC_PAWNS)
		{
			for (auto color = FIRST_PLAYER_INDEX; color < PLAYER_COUNT; color++)
			{
				auto index = std::min(piece_count[color][PAWN_INDEX], DYNAMIC_PAWNS_LAST_INDEX); // Grenzen einhalten
				*dynamic_pawns[color] += piece_count[color][PAWN_INDEX] * MATERIAL_DYNAMIC_PAWNS[index];
			}
		}

		// Piece Square Table für König hinzufügen
 		if (evaluationFeatureFlags & EVAL_FT_PIECE_SQUARE_TABLE && pos_game_phase != GamePhase::Opening)
		{
			UINT64 table_index[PLAYER_COUNT] =
			{
				king_pos[WHITE_INDEX].first + ((COLUMNS * king_pos[WHITE_INDEX].second)),
				king_pos[BLACK_INDEX].first + ((COLUMNS * king_pos[BLACK_INDEX].second))
			};

			float factor[PLAYER_COUNT] = { 0.0f };

			switch (pos_game_phase)
			{
			case GamePhase::Mid: 
				factor[WHITE_INDEX] = PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_MID_GAME[table_index[WHITE_INDEX]];
				factor[BLACK_INDEX] = PIECE_SQUARE_TABLE_KING_MID_GAME_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KING_MID_GAME)[table_index[BLACK_INDEX]];
				break;
			case GamePhase::End: 
				factor[WHITE_INDEX] = PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT * PIECE_SQUARE_TABLE_KING_END_GAME[table_index[WHITE_INDEX]];
				factor[BLACK_INDEX] = PIECE_SQUARE_TABLE_KING_END_GAME_WEIGHT * MIRROR_PIECE_SQUARE_TABLE(PIECE_SQUARE_TABLE_KING_END_GAME)[table_index[BLACK_INDEX]];
				break;
			}
			*square_table[WHITE_INDEX] += piece_count[WHITE_INDEX][KING_INDEX] * factor[WHITE_INDEX];
			*square_table[BLACK_INDEX] += piece_count[BLACK_INDEX][KING_INDEX] * factor[BLACK_INDEX];
		}

		// Läuferpaar aktiviert?
		if (evaluationFeatureFlags & EVAL_FT_BISHOP_PAIR)
		{
			for (auto color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				if (piece_count[color_index][BISHOP_INDEX] >= MIN_BISHOP_COUNT_PRECONDITION_BONUS)
					*bishop_pair[color_index] += BISHOP_PAIR_BONUS * BISHOP_PAIR_BONUS_WEIGHT;
			}
		}

		// Piece Mobility hinzufügen:
		if (evaluationFeatureFlags & EVAL_FT_PIECE_MOBILITY)
		{
			for (INT32 color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
			{
				for (INT32 type_index = FIRST_PIECE_TYPES_INDEX; type_index < MAX_PIECE_TYPES; type_index++)
				{
					auto factor = 1.0f;
					switch (type_index)
					{
					case PAWN_INDEX: factor = PIECE_MOBILITY_PAWN_WEIGHT; break;
					case KNIGHT_INDEX: factor = PIECE_MOBILITY_KNIGHT_WEIGHT; break;
					case BISHOP_INDEX: factor = PIECE_MOBILITY_BISHOP_WEIGHT; break;
					case ROOK_INDEX: factor = PIECE_MOBILITY_ROOK_WEIGHT; break;
					case QUEEN_INDEX: factor = PIECE_MOBILITY_QUEEN_WEIGHT; break;
					case KING_INDEX: factor = PIECE_MOBILITY_KING_WEIGHT; break;
					}
					*piece_mobility[color_index] += PIECE_MOBILITY_WEIGHT * factor * possible_moves[color_index][type_index];
				}
			}
		}

		// Materialwerte summieren
		for (INT32 color_index = FIRST_PLAYER_INDEX; color_index < PLAYER_COUNT; color_index++)
		{
		#if OWL_USE_EVALUATION_COMPONENT_REPRESENTAION==true
			data[color_index].calculateScore();
		#else
			_score[color_index] += _material[color_index] + _game_phase[color_index] + _square_table[color_index]
				+ _piece_mobility[color_index] + _pawn_structure[color_index] + _bishop_pair[color_index] + _dynamic_pawns[color_index];
		#endif
		}

		// Je nach Spieler score der Spieler voneinander abziehen und zurückgeben
	#if OWL_USE_EVALUATION_COMPONENT_REPRESENTAION==true
		EVALUATION_VALUE final_score = enginePlayer == PLAYER_WHITE ?
			data[WHITE_INDEX] - data[BLACK_INDEX] :
			data[BLACK_INDEX] - data[WHITE_INDEX];
	#else
		EVALUATION_VALUE final_score = enginePlayer == PLAYER_WHITE ?
			_score[WHITE_INDEX] - _score[BLACK_INDEX] :
			_score[BLACK_INDEX] - _score[WHITE_INDEX];
	#endif
		return final_score;
	}

	BOOL ChessEvaluation::isDoublePawn(const Position& position, INT32 x, INT32 y)
	{
		if (isPieceEqualOnOffset(position, x, y, 0, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, 0, -1)) return true;

		return false;
	}
	BOOL ChessEvaluation::isConnectedPawn(const Position& position, INT32 x, INT32 y)
	{
		if (isPieceEqualOnOffset(position, x, y, 1, 0)) return true;		
		if (isPieceEqualOnOffset(position, x, y, -1, 0)) return true;

		return false;
	}

	BOOL ChessEvaluation::isBackwardsPawn(const Position& position, INT32 x, INT32 y)
	{
		// Hinweis: isConnectedPawn(...) muss vorher false sein
		auto dir = position.getPlayer();

		// Gegnerischer Bauer vorne
		if (isPieceEnemyPawnOnOffset(position, x, y, 0, dir)) return true;
		if (isPieceEnemyPawnOnOffset(position, x, y, 1, 2 * dir)) return true;
		if (isPieceEnemyPawnOnOffset(position, x, y, -1, 2 * dir)) return true;

		return false;
	}
	BOOL ChessEvaluation::isPassedPawn(const Position& position, INT32 x, INT32 y)
	{
		auto enemy_pawn = position[y][x] == WHITE_PAWN ? WHITE_PAWN: BLACK_PAWN;

		for (INT32 i = 0; i < ROWS; i++)
		{
			if (i == y) continue; // Eigenes Feld ignorieren
			if (ChessValidation::isInsideChessboard(x, i))
			{
				if (position[i][x] == enemy_pawn) return false;
			}
		}

		return true;
	}
	BOOL ChessEvaluation::isChainPawn(const Position& position, INT32 x, INT32 y)
	{
		if (isPieceEqualOnOffset(position, x, y, 1, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, 1, -1)) return true;
		if (isPieceEqualOnOffset(position, x, y, -1, 1)) return true;
		if (isPieceEqualOnOffset(position, x, y, -1, -1)) return true;

		return false;
	}

	BOOL ChessEvaluation::isPieceEqualOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset)
	{
		if (ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
	BOOL ChessEvaluation::isPieceEnemyPawnOnOffset(const Position& position, INT32 x, INT32 y, INT32 xOffset, INT32 yOffset)
	{
		auto start_color = GetPlayerIndexByPositionPlayer(position.getPlayer());
		auto target_color = start_color + 1 % PLAYER_COUNT;

		auto start_pawn = PIECES[start_color][PAWN_INDEX];
		auto target_pawn = PIECES[target_color][PAWN_INDEX];

		if (ChessValidation::isInsideChessboard(x + xOffset, y + yOffset))
		{
			if (position[y][x] == start_pawn && target_pawn == position[y + yOffset][x + xOffset]) return true;
		}

		return false;
	}
	INT32 ChessEvaluation::GetPlayerIndexByPositionPlayer(INT32 currentPlayerOfPosition)
	{
		return currentPlayerOfPosition == PLAYER_WHITE ? WHITE_INDEX : BLACK_INDEX;
	}
	UCHAR ChessEvaluation::GetEnemyPiece(INT32 currentPlayerOfPosition, INT32 pieceIndex)
	{
		auto&& enemy_color = GetPlayerIndexByPositionPlayer(-currentPlayerOfPosition);
		return PIECES[enemy_color][pieceIndex];
	}
}