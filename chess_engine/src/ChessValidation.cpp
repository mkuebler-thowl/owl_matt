#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include "ChessUtility.hpp"
#include "ChessEvaluation.hpp"

#include <string>
#include <iostream>

namespace owl
{
	MOVE_LIST ChessValidation::s_data = MOVE_LIST();

	VOID ChessValidation::init()
	{
		s_data.reserve(MAX_MOVES_PER_PLY_BOUND);
	}
	MOVE_LIST ChessValidation::getValidMoves(Position& position, short player)
	{
		s_data.clear();

		for (UINT16 y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (UINT16 x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = position[y][x];
				if (piece == EMPTY_FIELD) continue;

				auto piece_color = GET_PLAYER_INDEX_BY_PIECE(piece);
				auto piece_player = piece_color == WHITE_INDEX ? PLAYER_WHITE : PLAYER_BLACK;

				if (player == piece_player)
				{
					auto piece_type = GET_PIECE_INDEX_BY_TYPE(piece);

					switch (piece_type)
					{
					case PAWN_INDEX: {
						getValidPawnMoves(position, x, y, player);
						break; }
					case KNIGHT_INDEX: {
						getValidKnightMoves(position, x, y, player);
						break; }
					case KING_INDEX: {
						getValidKingMoves(position, x, y, player);
						
						break; }
					case ROOK_INDEX: {
						getValidRookMoves(position, x, y, player);
						break; }
					case BISHOP_INDEX: {
						getValidBishopMoves(position, x, y, player);
						break; }
					case QUEEN_INDEX: {
						getValidRookMoves(position, x, y, player);
						getValidBishopMoves(position, x, y, player);
						break; }
					}
				}
			}
		}

		// Schachmatt abfragen
		// Da jeder Zug, der ein "Schach" auslöst entfernt wird, gilt das natürlich auch für alle Folgezüge, die eine "Schach"-Situation nicht auflösen können.
		// Deshalb reicht es abzufragen, ob der König im Schach steht und keine Züge mehr zur Verfügung stehen
		evaluateCheckmate(position, player, s_data.empty());

		return s_data;
	}
	UINT16 ChessValidation::countPossibleMovesOnField(Position& position, INT32 x, INT32 y)
	{
		//MOVE_LIST moves;
		s_data.clear();

		switch (position[y][x])
		{
		case WHITE_PAWN: getValidPawnMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_PAWN: getValidPawnMoves(position, x, y, PLAYER_BLACK); break;
		case WHITE_KNIGHT:getValidKnightMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_KNIGHT: getValidKnightMoves(position, x, y, PLAYER_BLACK); break;
		case WHITE_BISHOP: getValidBishopMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_BISHOP: getValidBishopMoves(position, x, y, PLAYER_BLACK); break;
		case WHITE_ROOK: getValidRookMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_ROOK: getValidRookMoves(position, x, y, PLAYER_BLACK); break;
		case WHITE_QUEEN: getValidBishopMoves(position, x, y, PLAYER_WHITE); getValidRookMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_QUEEN: getValidBishopMoves(position, x, y, PLAYER_BLACK); getValidRookMoves(position, x, y, PLAYER_BLACK); break;
		case WHITE_KING: getValidKingMoves(position, x, y, PLAYER_WHITE); break;
		case BLACK_KING: getValidKingMoves(position, x, y, PLAYER_BLACK); break;
		}
		return s_data.size();
	}

	BOOL ChessValidation::isKingInCheckAfterMove(Position& position, short player, const Move& move)
	{
		position.applyMove(move);
		auto value = isKingInCheck(position, player);
		position.undoLastMove();

		return value;
	}

	BOOL ChessValidation::isKingInCheck(Position& position, short player)
	{
		CHAR king = PIECES[ChessEvaluation::GetPlayerIndexByPositionPlayer(player)][KING_INDEX];
		INT32 king_x, king_y;
		BOOL king_found = false;

		// König suchen
		for (INT32 y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (INT32 x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				if (position[y][x] == king)
				{
					king_x = x;
					king_y = y;
					y = ROWS; // Grenze überschreiten (outer-break)
					king_found = true;
					break;
				}
			}
		}

		// Falls der König nicht gefunden wird:
		if (!king_found)
		{
			return false;
		}

		if (checkKingPawns(king_x, king_y, position, player)) return true;
		if (checkKingAxis(king_x, king_y, position, player)) return true;
		if (checkKingDiagonal(king_x, king_y, position, player)) return true;
		if (checkKingKnights(king_x, king_y, position, player)) return true;

		return false;
	}

	BOOL ChessValidation::isPlaceInCheck(const Position& position, INT32 x, INT32 y, short player)
	{
		if (checkKingPawns(x, y, position, player)) return true;
		if (checkKingAxis(x, y, position, player)) return true;
		if (checkKingDiagonal(x, y, position, player)) return true;
		if (checkKingKnights(x, y, position, player)) return true;

		return false;
	}

	BOOL ChessValidation::checkKingDiagonal(INT32 king_x, INT32 king_y, const Position& position, short player)
	{
		CHAR enemy_queen = player == PLAYER_WHITE ? 'q' : 'Q';
		CHAR enemy_bishop = player == PLAYER_WHITE ? 'b' : 'B';

		// diagonal links oben:
		if (king_x > 0 && king_y > 0)
		{
			for (INT32 i = 1; i <= std::min(king_x, king_y); i++)
			{
				auto place = position[king_y - i][king_x - i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				else if (place != EMPTY_FIELD) break;
			}
		}
		// diagonal rechts unten:
		if (king_x < 7 && king_y < 7)
		{
			for (INT32 i = 1; i <= std::min(7 - king_x, 7 - king_y); i++)
			{
				auto place = position[king_y + i][king_x + i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}
		// diagonal rechts oben:
		if (king_x < 7 && king_y > 0)
		{
			for (INT32 i = 1; i <= std::min(king_y, 7 - king_x); i++)
			{
				auto place = position[king_y - i][king_x + i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}
		// diagonal links unten:
		if (king_x > 0 && king_y < 7)
		{
			for (INT32 i = 1; i <= std::min(7 - king_y, king_x); i++)
			{
				auto place = position[king_y + i][king_x - i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}

		return false;
	}
	BOOL ChessValidation::checkKingAxis(INT32 king_x, INT32 king_y, const Position& position, short player)
	{
		UINT16 enemy_player_index = player == PLAYER_WHITE ? BLACK_INDEX : WHITE_INDEX;

		CHAR enemy_queen = PIECES[enemy_player_index][QUEEN_INDEX];
		CHAR enemy_rook = PIECES[enemy_player_index][ROOK_INDEX];

		// horizontal:
		// links vom König:
		if (king_x > FIRST_COLUMN_INDEX)
		{
			for (INT32 x = king_x - 1; x >= FIRST_COLUMN_INDEX; x--)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// rechts vom König:
		if (king_x < LAST_COLUMN_INDEX)
		{
			for (INT32 x = king_x + 1; x <= LAST_COLUMN_INDEX; x++)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != EMPTY_FIELD) break;
			}
		}

		// vertikal:
		// oben vom König:
		if (king_y > FIRST_COLUMN_INDEX)
		{
			for (INT32 y = king_y - 1; y >= FIRST_ROW_INDEX; y--)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != EMPTY_FIELD) break;
			}
		}
		// unten vom König:
		if (king_y < LAST_COLUMN_INDEX)
		{
			for (INT32 y = king_y + 1; y <= LAST_ROW_INDEX; y++)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != EMPTY_FIELD) break;
			}
		}
		return false;
	}
	BOOL ChessValidation::checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, short player)
	{
		CHAR enemy_knight = player == PLAYER_WHITE ? BLACK_KNIGHT : WHITE_KNIGHT;

		for (auto direction : MOVE_DIR_KNIGHT)
		{
			auto x = king_x + direction[FIRST];
			auto y = king_y + direction[SECOND];

			if (isInsideChessboard(x,y))
			{
				if (position[y][x] == enemy_knight) return true;
			}
		}

		return false;
	}
	BOOL ChessValidation::checkKingPawns(INT32 king_x, INT32 king_y, const Position& position, short player)
	{
		CHAR enemy_pawn = ChessEvaluation::GetEnemyPiece(player, PAWN_INDEX);
		constexpr short king_offset = 1;

		// Weißen König auf schwarze Bauern überprüfen:
		//  p p
		//   K
		if (player == PLAYER_WHITE)
		{
			// links oben:
			if (king_y > FIRST_ROW_INDEX && king_x > FIRST_COLUMN_INDEX)
			{
				auto piece = position[king_y - king_offset][king_x - king_offset];
				if (piece == enemy_pawn) return true;
			}
			// rechts oben:
			if (king_y > FIRST_ROW_INDEX && king_x < LAST_COLUMN_INDEX)
			{
				auto piece = position[king_y - king_offset][king_x + king_offset];
				if (piece == enemy_pawn) return true;
			}
		}
		else if (player == PLAYER_BLACK)
		{
			// links unten:
			if (king_y < LAST_ROW_INDEX && king_x > FIRST_COLUMN_INDEX)
			{
				auto piece = position[king_y + king_offset][king_x - king_offset];
				if (piece == enemy_pawn) return true;
			}
			// rechts unten:
			if (king_y < LAST_ROW_INDEX && king_x < LAST_COLUMN_INDEX)
			{
				auto piece = position[king_y + king_offset][king_x + king_offset];
				if (piece == enemy_pawn) return true;
			}
		}

		return false;
	}
	VOID ChessValidation::evaluateCheckmate(Position& position, short player, BOOL noValidMoves)
	{
		if (!noValidMoves) return;

		// Sieg & Niederlage
		if (isKingInCheck(position, player))
		{
			auto state = player == PLAYER_WHITE ? GameState::PlayerBlackWins : GameState::PlayerWhiteWins;
			position.setGameState(state);
		}
		// Pattsituation (Spieler kann keine Bewegung durchführen)
		else
		{
			position.setGameState(GameState::Remis);
		}
	}

	VOID ChessValidation::getValidPawnMoves(Position& position, INT32 x, INT32 y, short player)
	{
		//MOVE_LIST moves;
		//moves.reserve(MAX_MOVES_PER_PAWN);
		auto pawn_direction = player;
		std::string enemies = getEnemyPiecesString(player);
		std::string promotion_str = std::string(player == PLAYER_WHITE ? WHITE_PROMOTION_PIECES : BLACK_PROMOTION_PIECES);

		if (y > FIRST_ROW_INDEX && player == PLAYER_WHITE || y < LAST_ROW_INDEX && player == PLAYER_BLACK)
		{
			// Schritt nach vorne 
			if (position[y - pawn_direction][x] == EMPTY_FIELD)
			{
				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x;
				move.targetY = y - pawn_direction;
				move.capture = false;

				if (!isKingInCheckAfterMove(position, player, move))
				{
					// Bauernumwandlung?
					if (y - pawn_direction == 0) {

						for (auto c : promotion_str)
						{
							move.promotion = c;
							s_data.emplace_back(move);
						}
					}
					else
					{
						s_data.emplace_back(move);
					}
				}
			}

			// 2-Schritte am Anfang?
			if ((y == 6 && player == PLAYER_WHITE && position[4][x] == EMPTY_FIELD && position[5][x] == EMPTY_FIELD)
				|| (y == 1 && player == PLAYER_BLACK && position[2][x] == EMPTY_FIELD && position[3][x] == EMPTY_FIELD))
			{
				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x;
				move.targetY = y - 2 * pawn_direction;
				move.capture = false;
				move.promotion = 0;

				if (!isKingInCheckAfterMove(position, player, move))
					s_data.emplace_back(move);
			}

			// Diagonal Schlagen:
			if (x > FIRST_COLUMN_INDEX)
			{	
				// links oben/unten
				if (isInsideChessboard(x - 1, y - pawn_direction))
				{
					if (enemies.find(position[y - pawn_direction][x - 1]) != std::string::npos)
					{
						Move move;
						move.startX = x;
						move.startY = y;
						move.targetX = x - 1;
						move.targetY = y - pawn_direction;
						move.capture = true;
						move.promotion = 0;

						if (!isKingInCheckAfterMove(position, player, move))
							s_data.emplace_back(move);
					}
				}

				// rechts oben/unten
				if (isInsideChessboard(x + 1, y - pawn_direction))
				{
					if (enemies.find(position[y - pawn_direction][x + 1]) != std::string::npos)
					{
						Move move;
						move.startX = x;
						move.startY = y;
						move.targetX = x + 1;
						move.targetY = y - pawn_direction;
						move.capture = true;
						move.promotion = 0;

						if (!isKingInCheckAfterMove(position, player, move))
							s_data.emplace_back(move);
					}
				}

				// En Passant überprüfen
				if (position.isEnPassant())
				{
					auto enPassant = position.getEnPassant();
					if (y - pawn_direction == enPassant.second)
					{
						// links oben/unten
						if (enPassant.first == x - 1)
						{
							Move move;
							move.startX = x;
							move.startY = y;
							move.targetX = x - 1;
							move.targetY = y - pawn_direction;
							move.capture = true;
							move.enPassantCapture = true;
							move.promotion = 0;

							if (!isKingInCheckAfterMove(position, player, move))
								s_data.emplace_back(move);
						}

						// rechts oben/unten
						if (enPassant.first == x + 1)
						{
							Move move;
							move.startX = x;
							move.startY = y;
							move.targetX = x + 1;
							move.targetY = y - pawn_direction;
							move.capture = true;
							move.enPassantCapture = true;
							move.promotion = 0;

							if (!isKingInCheckAfterMove(position, player, move))
								s_data.emplace_back(move);
						}
					}
				}
			}
		}

		return;
	}

	VOID ChessValidation::getValidKnightMoves(Position& position, INT32 x, INT32 y, short player)
	{
		//MOVE_LIST moves;
		//moves.reserve(MAX_MOVES_PER_KNIGHT);

		std::string enemies = getEnemyPiecesString(player);

		for (auto pair : MOVE_DIR_KNIGHT)
		{
			if (isInsideChessboard(x + pair[FIRST], y + pair[SECOND]))
			{
				auto piece = position[y + pair[SECOND]][x + pair[FIRST]];
				auto capture = enemies.find(piece) != std::string::npos;
				if (piece == ' ' || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = x + pair[FIRST];
					move.targetY = y + pair[SECOND];
					move.capture = capture;

					// Prüfen ob der König nach dem Zug nicht im Schach steht:
					if(!isKingInCheckAfterMove(position, player, move))
						s_data.emplace_back(move);
				}
			}
		}

		return;
	}

	VOID ChessValidation::getValidKingMoves(Position& position, INT32 x, INT32 y, short player)
	{
		//MOVE_LIST moves;
		//moves.reserve(MAX_MOVES_PER_KING);

		std::string enemies = getEnemyPiecesString(player);

		for (auto pair : MOVE_DIR_KING)
		{
			auto first = pair[FIRST];
			auto second = pair[SECOND];
			if (isInsideChessboard(x + first, y + second))
			{
				auto piece = position[y + second][x + first];
				auto capture = enemies.find(piece) != std::string::npos;
				if (piece == EMPTY_FIELD || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = x + first;
					move.targetY = y + second;
					move.capture = capture;

					// Prüfen ob der König nach dem Zug nicht im Schach steht:
					if (!isKingInCheckAfterMove(position, player, move))
						s_data.emplace_back(move);
				}
			}
		}

		// Rochade 
		if (player == PLAYER_WHITE)
		{
			if (position.getWhiteCastlingShort() 
				&& isPlaceInCheck(position, 5, LAST_ROW_INDEX, player)
				&& isPlaceInCheck(position, 6, LAST_ROW_INDEX, player)
				&& position[LAST_ROW_INDEX][5] == EMPTY_FIELD
				&& position[LAST_ROW_INDEX][6] == EMPTY_FIELD)
			{
				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x+2;
				move.targetY = y;
				move.castlingShort = true;

				s_data.emplace_back(move);
			}
			if (position.getWhiteCastlingLong()
				&& isPlaceInCheck(position, 2, LAST_ROW_INDEX, player)
				&& isPlaceInCheck(position, 3, LAST_ROW_INDEX, player)
				&& position[LAST_ROW_INDEX][3] == EMPTY_FIELD
				&& position[LAST_ROW_INDEX][2] == EMPTY_FIELD
				&& position[LAST_ROW_INDEX][1] == EMPTY_FIELD)
			{
				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x - 2;
				move.targetY = y;
				move.castlingLong = true;

				s_data.emplace_back(move);
			}
		}
		else if (player == PLAYER_BLACK)
		{
			if (position.getBlackCastlingShort()
				&& isPlaceInCheck(position, 5, FIRST_ROW_INDEX, player)
				&& isPlaceInCheck(position, 6, FIRST_ROW_INDEX, player)
				&& position[FIRST_ROW_INDEX][5] == EMPTY_FIELD
				&& position[FIRST_ROW_INDEX][6] == EMPTY_FIELD)
			{

				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x + 2;
				move.targetY = y;
				move.castlingShort = true;

				s_data.emplace_back(move);
			}
			if (position.getBlackCastlingLong()
				&& isPlaceInCheck(position, 2, FIRST_ROW_INDEX, player)
				&& isPlaceInCheck(position, 3, FIRST_ROW_INDEX, player)
				&& position[FIRST_ROW_INDEX][3] == EMPTY_FIELD
				&& position[FIRST_ROW_INDEX][2] == EMPTY_FIELD
				&& position[FIRST_ROW_INDEX][1] == EMPTY_FIELD)
			{
				Move move;
				move.startX = x;
				move.startY = y;
				move.targetX = x - 2;
				move.targetY = y;
				move.castlingLong = true;

				s_data.emplace_back(move);
			}
		}

		return;
	}

	VOID ChessValidation::getValidRookMoves(Position& position, INT32 x, INT32 y, short player)
	{
		////MOVE_LIST moves;
		////moves.reserve(MAX_MOVES_PER_KING);
		std::string enemies = getEnemyPiecesString(player);
		
		for (auto direction : MOVE_DIR_ROOK)
		{
			continueValidMovesOnLine(position, x, y, enemies, direction[FIRST], direction[SECOND]);
			//moves.insert(moves.end(), tmp.begin(), tmp.end());
			//std::move(tmp.begin(), tmp.end(), std::back_inserter(moves));
		}

		return;
	}

	VOID ChessValidation::getValidBishopMoves(Position& position, INT32 x, INT32 y, short player)
	{
		//MOVE_LIST moves;
		//moves.reserve(MAX_MOVES_PER_BISHOP);

		std::string enemies = getEnemyPiecesString(player);

		for (auto direction : MOVE_DIR_BISHOP)
		{
			continueValidMovesOnLine(position, x, y, enemies, direction[FIRST], direction[SECOND]);
			//moves.insert(moves.end(), tmp.begin(), tmp.end());
			//std::move(tmp.begin(), tmp.end(), std::back_inserter(moves));
		}

		return;
	}

	VOID ChessValidation::continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir)
	{
		//MOVE_LIST moves;
		//moves.reserve(MAX_LINE_LENGTH);
		BOOL line_empty = true;

		INT32 offset = 1;

		while (line_empty)
		{
			INT32 line_x = x + xDir * offset;
			INT32 line_y = y + yDir * offset;

			if (isInsideChessboard(line_x, line_y))
			{
				auto place = position[line_y][line_x];
				auto capture = enemies_string.find(place) != std::string::npos;

				if (place == EMPTY_FIELD || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = line_x;
					move.targetY = line_y;
					move.capture = capture;

					if (!isKingInCheckAfterMove(position, position.getPlayer(), move))
						s_data.emplace_back(move);

					if (capture) line_empty = false;
				}
				else
				{
					line_empty = false;
				}
			}
			else
			{
				line_empty = false;
			}
			offset++;
		}

		return;
	}

	std::string ChessValidation::getEnemyPiecesString(short player)
	{
		return std::string(player == PLAYER_WHITE ? BLACK_PIECES : WHITE_PIECES);
	}

	BOOL ChessValidation::isInsideChessboard(INT32 x, INT32 y)
	{
		return x>=FIRST_ROW_INDEX && x<COLUMNS && y>=FIRST_ROW_INDEX && y<ROWS;
	}
}