#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"
#include "ChessEvaluation.hpp"

#include <string>
#include <iostream>
namespace owl
{
	MOVE_LIST ChessValidation::getValidMoves(Position& position, short player)
	{
		MOVE_LIST moves;

		for (INT32 y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (INT32 x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = position[y][x];

				auto piece_color = GET_PLAYER_INDEX_BY_PIECE(piece);
				auto piece_player = piece_color == WHITE_INDEX ? PLAYER_WHITE : PLAYER_BLACK;

				if (player == piece_player)
				{
					auto piece_type = GET_PIECE_INDEX_BY_TYPE(piece);

					switch (piece_type)
					{
					case PAWN_INDEX: {
						auto tmp = getValidPawnMoves(position, x, y, player);
						//moves.reserve(moves.size() + pawns.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						break; }
					case KNIGHT_INDEX: {
						auto tmp = getValidKnightMoves(position, x, y, player);
						//moves.reserve(moves.size() + knights.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						break; }
					case KING_INDEX: {
						auto tmp = getValidKingMoves(position, x, y, player);
						//moves.reserve(moves.size() + kings.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						break; }
					case ROOK_INDEX: {
						auto tmp = getValidRookMoves(position, x, y, player);
						//moves.reserve(moves.size() + rooks.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						break; }
					case BISHOP_INDEX: {
						auto tmp = getValidBishopMoves(position, x, y, player);
						//moves.reserve(moves.size() + bishops.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						break; }
					case QUEEN_INDEX: {
						auto tmp = getValidRookMoves(position, x, y, player);
						auto tmp2 = getValidBishopMoves(position, x, y, player);
						//moves.reserve(moves.size() + axis.size() + diagonal.size());
						moves.insert(moves.end(), tmp.begin(), tmp.end());
						moves.insert(moves.end(), tmp2.begin(), tmp2.end());
						break; }
					}
				}
			}
		}

		// Schachmatt abfragen
		// Da jeder Zug, der ein "Schach" auslöst entfernt wird, gilt das natürlich auch für alle Folgezüge, die eine "Schach"-Situation nicht auflösen können.
		// Deshalb reicht es abzufragen, ob der König im Schach steht und keine Züge mehr zur Verfügung stehen
		evaluateCheckmate(position, player, moves.empty());

		return moves;
	}
	UINT16 ChessValidation::countPossibleMovesOnField(Position& position, INT32 x, INT32 y)
	{
		UINT16 count = 0;

		switch (position[y][x])
		{
		case WHITE_PAWN: count += getValidPawnMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_PAWN: count += getValidPawnMoves(position, x, y, PLAYER_BLACK).size(); break;
		case WHITE_KNIGHT: count += getValidKnightMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_KNIGHT: count += getValidKnightMoves(position, x, y, PLAYER_BLACK).size(); break;
		case WHITE_BISHOP: count += getValidBishopMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_BISHOP: count += getValidBishopMoves(position, x, y, PLAYER_BLACK).size(); break;
		case WHITE_ROOK: count += getValidRookMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_ROOK: count += getValidRookMoves(position, x, y, PLAYER_BLACK).size(); break;
		case WHITE_QUEEN: count += getValidBishopMoves(position, x, y, PLAYER_WHITE).size() + getValidRookMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_QUEEN: count += getValidBishopMoves(position, x, y, PLAYER_BLACK).size() + getValidRookMoves(position, x, y, PLAYER_BLACK).size(); break;
		case WHITE_KING: count += getValidKingMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BLACK_KING: count += getValidKingMoves(position, x, y, PLAYER_BLACK).size(); break;
		}
		return count;
	}
	Position ChessValidation::applyMove(Position& position, const Move& move)
	{
		//auto pos = position;
		//pos.applyMove(move);
		//return pos;

		auto pos = position;
		auto figure = position[move.startY][move.startX];

		pos[move.targetY][move.targetX] = figure;
		pos[move.startY][move.startX] = EMPTY_FIELD;
		
		// Ist der Zug eine Rochade? => Turm bewegen
		if (move.castlingLong)
		{
			auto rook = position[move.startY][FIRST_COLUMN_INDEX];
			pos[move.startY][FIRST_COLUMN_INDEX] = EMPTY_FIELD;
			pos[move.startY][3] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = position[move.startY][COLUMNS-1];
			pos[move.startY][LAST_COLUMN_INDEX] = EMPTY_FIELD;
			pos[move.startY][5] = rook;
		}

		// Wurde die En Passant-Capture eingelöst? Lösche richtiges Feld
		if (move.enPassantCapture && (move.targetY == 2 || move.targetY == 5)) 
		{
			pos[move.startY][move.targetX] = EMPTY_FIELD;
		}

		pos.addPlyCount();
		pos.changePlayer();

		// En Passant zurücksetzen:
		if (position.isEnPassant())
		{
			pos.resetEnPassant();
		}

		// 50-Züge-Regel: Capture abfragen & Bauernbewegung
		if (figure == WHITE_PAWN || figure == BLACK_PAWN || move.capture)
		{
			pos.resetPlyCount();
		}

		// En passant für die Folgeposition bestimmen:
		if ((figure == BLACK_PAWN && move.startY == 1 && move.targetY == 3)
			|| (figure == WHITE_PAWN && move.startY == 6 && move.targetY == 4))
		{
			auto dir = move.startY == 1 ? 1 : -1;
			pos.setEnPassant(move.startX, move.startY + dir);
		}

		// 100 Halbzüge woanders informieren; auswerten
		if (pos.getPlyCount() >= 100) // 100 Halbzüge bzw. 50 Züge
		{
			pos.setGameState(GameState::Remis);
		}

		// Rochieren gegebenfalls deaktivieren
		if (pos.getWhiteCastlingShort() || pos.getWhiteCastlingLong() || pos.getBlackCastlingLong() || pos.getBlackCastlingShort())
		{
			switch (figure)
			{
			case WHITE_KING:
				pos.resetWhiteCastlingLong();
				pos.resetWhiteCastlingShort();
				break;
			case BLACK_KING:
				pos.resetBlackCastlingLong();
				pos.resetBlackCastlingShort();
				break;
			case WHITE_ROOK:
				if (move.startX == FIRST_COLUMN_INDEX) pos.resetWhiteCastlingLong();
				else if (move.startX == LAST_COLUMN_INDEX) pos.resetWhiteCastlingShort();
				break;
			case BLACK_ROOK:
				if (move.startX == FIRST_COLUMN_INDEX) pos.resetBlackCastlingLong();
				else if (move.startX == LAST_COLUMN_INDEX) pos.resetBlackCastlingShort();
			}
		}

		return pos;
	}

	BOOL ChessValidation::isKinginCheckAfterMove(Position& position, short player, const Move& move)
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
			for (INT32 i = std::min(king_x, king_y); i >= 0; i--)
			{
				auto place = position[king_y - i][king_x - i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
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
		CHAR enemy_queen = player == PLAYER_WHITE ? 'q' : 'Q';
		CHAR enemy_rook = player == PLAYER_WHITE ? 'r' : 'R';

		// horizontal:
		// links vom König:
		if (king_x > 0)
		{
			for (INT32 x = king_x - 1; x >= 0; x--)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// rechts vom König:
		if (king_x < 7)
		{
			for (INT32 x = king_x + 1; x <= 7; x++)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}

		// vertikal:
		// oben vom König:
		if (king_y > 0)
		{
			for (INT32 y = king_y - 1; y >= 0; y--)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// unten vom König:
		if (king_y < 7)
		{
			for (INT32 y = king_y + 1; y <= 7; y++)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		return false;
	}
	BOOL ChessValidation::checkKingKnights(INT32 king_x, INT32 king_y, const Position& position, short player)
	{
		CHAR enemy_knight = player == PLAYER_WHITE ? 'n' : 'N';
		std::vector<PAIR<INT32, INT32>> directions = { {-1,-2}, {2,-1}, {2,1}, {1,2}, {-1,2}, {-2,1}, {-2,-1}, {-1,-2} };

		for (auto direction : directions)
		{
			auto x = king_x + direction.first;
			auto y = king_y + direction.second;

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
			if (king_y < LAST_ROW_INDEX && king_x < FIRST_COLUMN_INDEX)
			{
				auto piece = position[king_y + king_offset][king_x + king_offset];
				if (piece == enemy_pawn) return true;
			}
		}

		return false;
	}
	VOID ChessValidation::evaluateCheckmate(Position& position, short player, BOOL noValidMoves)
	{
		if (noValidMoves)
		{
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
	}

	MOVE_LIST ChessValidation::getValidPawnMoves(Position& position, INT32 x, INT32 y, short player)
	{
		MOVE_LIST moves;
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

				// Bauernumwandlung?
				if (y - pawn_direction == 0) {

					for (auto c : promotion_str)
					{
						move.promotion = c;
						if (!isKinginCheckAfterMove(position, player, move))
							moves.push_back(move);
					}
				}
				else
				{
					if (!isKinginCheckAfterMove(position, player, move))
						moves.push_back(move);
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

				if (!isKinginCheckAfterMove(position, player, move))
					moves.push_back(move);
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

						if (!isKinginCheckAfterMove(position, player, move))
							moves.push_back(move);
					}
				}

				if (isInsideChessboard(x + 1, y - pawn_direction))
				{
					// rechts oben/unten
					if (enemies.find(position[y - pawn_direction][x + 1]) != std::string::npos)
					{
						Move move;
						move.startX = x;
						move.startY = y;
						move.targetX = x + 1;
						move.targetY = y - pawn_direction;
						move.capture = true;
						move.promotion = 0;

						if (!isKinginCheckAfterMove(position, player, move))
							moves.push_back(move);
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

							if (!isKinginCheckAfterMove(position, player, move))
								moves.push_back(move);
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

							if (!isKinginCheckAfterMove(position, player, move))
								moves.push_back(move);
						}
					}
				}
			}
		}

		return moves;
	}

	MOVE_LIST ChessValidation::getValidKnightMoves(Position& position, INT32 x, INT32 y, short player)
	{
		MOVE_LIST moves;
		std::vector<PAIR<INT32, INT32>> possible_places = { {1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1}, {-1,-2} };
		std::string enemies = getEnemyPiecesString(player);

		for (auto pair : possible_places)
		{
			if (isInsideChessboard(x + pair.first, y + pair.second))
			{
				auto piece = position[y + pair.second][x + pair.first];
				auto capture = enemies.find(piece) != std::string::npos;
				if (piece == ' ' || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = x + pair.first;
					move.targetY = y + pair.second;
					move.capture = capture;

					// Prüfen ob der König nach dem Zug nicht im Schach steht:
					if(!isKinginCheckAfterMove(position, player, move))
						moves.push_back(move);
				}
			}
		}

		return moves;
	}

	MOVE_LIST ChessValidation::getValidKingMoves(Position& position, INT32 x, INT32 y, short player)
	{
		MOVE_LIST moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<PAIR<INT32, INT32>> possible_places = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1} };

		for (auto pair : possible_places)
		{
			if (isInsideChessboard(x + pair.first, y + pair.second))
			{
				auto piece = position[y + pair.second][x + pair.first];
				auto capture = enemies.find(piece) != std::string::npos;
				if (piece == ' ' || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = x + pair.first;
					move.targetY = y + pair.second;
					move.capture = capture;

					// Prüfen ob der König nach dem Zug nicht im Schach steht:
					if (!isKinginCheckAfterMove(position, player, move))
						moves.push_back(move);
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

				moves.push_back(move);
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

				moves.push_back(move);
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

				moves.push_back(move);
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

				moves.push_back(move);
			}
		}

		return moves;
	}

	MOVE_LIST ChessValidation::getValidRookMoves(Position& position, INT32 x, INT32 y, short player)
	{
		MOVE_LIST moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<PAIR<INT32, INT32>> directions = { {0,-1}, {1,0}, {0,1}, {-1,0} };
		
		for (auto direction : directions)
		{
			auto tmp = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), tmp.begin(), tmp.end());
		}

		return moves;
	}

	MOVE_LIST ChessValidation::getValidBishopMoves(Position& position, INT32 x, INT32 y, short player)
	{
		MOVE_LIST moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<PAIR<INT32, INT32>> directions = { {-1,-1}, {1,-1}, {1,1}, {-1,1} };

		for (auto direction : directions)
		{
			auto tmp = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), tmp.begin(), tmp.end());
		}

		return moves;
	}

	MOVE_LIST ChessValidation::continueValidMovesOnLine(Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir)
	{
		MOVE_LIST moves;
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

					if (!isKinginCheckAfterMove(position, position.getPlayer(), move))
						moves.push_back(move);

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

		return moves;
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