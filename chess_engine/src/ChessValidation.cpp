#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"
#include "ChessEvaluation.hpp"

#include <string>
#include <iostream>
namespace owl
{
	MoveList ChessValidation::getValidMoves(const Position& position, short player)
	{
		MoveList moves;

		for (INT32 y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (INT32 x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = position[y][x];

				auto piece_color = GET_PIECE_COLOR_BY_TYPE(piece);
				auto piece_player = piece_color == WHITE ? PLAYER_WHITE : PLAYER_BLACK;

				if (player == piece_player)
				{
					auto piece_type = GET_PIECE_INDEX_BY_TYPE(piece);

					switch (piece_type)
					{
					case PAWN: {
						auto pawns = getValidPawnMoves(position, x, y, player);
						moves.insert(moves.end(), pawns.begin(), pawns.end());
						break; }
					case KNIGHT: {
						auto knights = getValidKnightMoves(position, x, y, player);
						moves.insert(moves.end(), knights.begin(), knights.end());
						break; }
					case KING: {
						auto kings = getValidKingMoves(position, x, y, player);
						moves.insert(moves.end(), kings.begin(), kings.end());
						break; }
					case ROOK: {
						auto rooks = getValidRookMoves(position, x, y, player);
						moves.insert(moves.end(), rooks.begin(), rooks.end());
						break; }
					case BISHOP: {
						auto bishops = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), bishops.begin(), bishops.end());
						break; }
					case QUEEN: {
						auto axis = getValidRookMoves(position, x, y, player);
						auto diagonal = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), axis.begin(), axis.end());
						moves.insert(moves.end(), diagonal.begin(), diagonal.end());
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
	UINT16 ChessValidation::countPossibleMovesOnField(const Position& position, INT32 x, INT32 y)
	{
		UINT16 count = 0;

		switch (position[y][x])
		{
		case PAWN_WHITE: count += getValidPawnMoves(position, x, y, PLAYER_WHITE).size(); break;
		case PAWN_BLACK: count += getValidPawnMoves(position, x, y, PLAYER_BLACK).size(); break;
		case KNIGHT_WHITE: count += getValidKnightMoves(position, x, y, PLAYER_WHITE).size(); break;
		case KNIGHT_BLACK: count += getValidKnightMoves(position, x, y, PLAYER_BLACK).size(); break;
		case BISHOP_WHITE: count += getValidBishopMoves(position, x, y, PLAYER_WHITE).size(); break;
		case BISHOP_BLACK: count += getValidBishopMoves(position, x, y, PLAYER_BLACK).size(); break;
		case ROOK_WHITE: count += getValidRookMoves(position, x, y, PLAYER_WHITE).size(); break;
		case ROOK_BLACK: count += getValidRookMoves(position, x, y, PLAYER_BLACK).size(); break;
		case QUEEN_WHITE: count += getValidBishopMoves(position, x, y, PLAYER_WHITE).size() + getValidRookMoves(position, x, y, PLAYER_WHITE).size(); break;
		case QUEEN_BLACK: count += getValidBishopMoves(position, x, y, PLAYER_BLACK).size() + getValidRookMoves(position, x, y, PLAYER_BLACK).size(); break;
		case KING_WHITE: count += getValidKingMoves(position, x, y, PLAYER_WHITE).size(); break;
		case KING_BLACK: count += getValidKingMoves(position, x, y, PLAYER_BLACK).size(); break;
		}
		return count;
	}
	Position ChessValidation::applyMove(const Position& position, const Move& move)
	{
		auto pos = position;
		auto figure = position[move.startY][move.startX];

		pos[move.targetY][move.targetX] = figure;
		pos[move.startY][move.startX] = EMPTY_PLACE;
		
		// Ist der Zug eine Rochade? => Turm bewegen
		if (move.castlingLong)
		{
			auto rook = position[move.startY][FIRST_COLUMN_INDEX];
			pos[move.startY][FIRST_COLUMN_INDEX] = EMPTY_PLACE;
			pos[move.startY][3] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = position[move.startY][COLUMNS-1];
			pos[move.startY][LAST_COLUMN_INDEX] = EMPTY_PLACE;
			pos[move.startY][5] = rook;
		}

		// Wurde die En Passant-Capture eingelöst? Lösche richtiges Feld
		if (move.enPassantCapture && (move.targetY == 2 || move.targetY == 5)) 
		{
			pos[move.startY][move.targetX] = EMPTY_PLACE;
		}

		pos.addPlyCount();
		pos.changePlayer();

		// En Passant zurücksetzen:
		if (position.isEnPassant())
		{
			pos.resetEnPassant();
		}

		// 50-Züge-Regel: Capture abfragen & Bauernbewegung
		if (figure == PAWN_WHITE || figure == PAWN_BLACK || move.capture)
		{
			pos.resetPlyCount();
		}

		// En passant für die Folgeposition bestimmen:
		if ((figure == PAWN_BLACK && move.startY == 1 && move.targetY == 3)
			|| (figure == PAWN_WHITE && move.startY == 6 && move.targetY == 4))
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
			case KING_WHITE:
				pos.resetWhiteCastlingLong();
				pos.resetWhiteCastlingShort();
				break;
			case KING_BLACK:
				pos.resetBlackCastlingLong();
				pos.resetBlackCastlingShort();
				break;
			case ROOK_WHITE:
				if (move.startX == FIRST_COLUMN_INDEX) pos.resetWhiteCastlingLong();
				else if (move.startX == LAST_COLUMN_INDEX) pos.resetWhiteCastlingShort();
				break;
			case ROOK_BLACK:
				if (move.startX == FIRST_COLUMN_INDEX) pos.resetBlackCastlingLong();
				else if (move.startX == LAST_COLUMN_INDEX) pos.resetBlackCastlingShort();
			}
		}

		return pos;
	}

	BOOL ChessValidation::isKinginCheckAfterMove(const Position& position, short player, const Move& move)
	{
		auto nextPosition = applyMove(position, move);
		return isKingInCheck(nextPosition, player);
	}
	BOOL ChessValidation::isKingInCheck(const Position& position, short player)
	{
		CHAR king = PIECES[ChessEvaluation::GetPlayerIndexByPositionPlayer(player)][KING];
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
		std::vector<std::pair<INT32, INT32>> directions = { {-1,-2}, {2,-1}, {2,1}, {1,2}, {-1,2}, {-2,1}, {-2,-1}, {-1,-2} };

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
		CHAR enemy_pawn = ChessEvaluation::GetEnemyPiece(player, PAWN);
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
	VOID ChessValidation::evaluateCheckmate(const Position& position, short player, BOOL noValidMoves)
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

	MoveList ChessValidation::getValidPawnMoves(const Position& position, INT32 x, INT32 y, short player)
	{
		MoveList moves;

		auto pawn_direction = player;
		std::string enemies = getEnemyPiecesString(player);
		std::string promotion_str = player == PLAYER_WHITE ? WHITE_PROMOTION_STR : BLACK_PROMOTION_STR;

		if (y > FIRST_ROW_INDEX && player == PLAYER_WHITE || y < LAST_ROW_INDEX && player == PLAYER_BLACK)
		{
			// Schritt nach vorne 
			if (position[y - pawn_direction][x] == EMPTY_PLACE)
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
			if ((y == 6 && player == PLAYER_WHITE && position[4][x] == EMPTY_PLACE && position[5][x] == EMPTY_PLACE)
				|| (y == 1 && player == PLAYER_BLACK && position[2][x] == EMPTY_PLACE && position[3][x] == EMPTY_PLACE))
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

	MoveList ChessValidation::getValidKnightMoves(const Position& position, INT32 x, INT32 y, short player)
	{
		MoveList moves;

		std::vector<std::pair<INT32, INT32>> possible_places = { {1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1}, {-1,-2} };
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

	MoveList ChessValidation::getValidKingMoves(const Position& position, INT32 x, INT32 y, short player)
	{
		MoveList moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<INT32, INT32>> possible_places = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1} };

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
				&& position[LAST_ROW_INDEX][5] == EMPTY_PLACE
				&& position[LAST_ROW_INDEX][6] == EMPTY_PLACE)
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
				&& position[LAST_ROW_INDEX][3] == EMPTY_PLACE
				&& position[LAST_ROW_INDEX][2] == EMPTY_PLACE
				&& position[LAST_ROW_INDEX][1] == EMPTY_PLACE)
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
				&& position[FIRST_ROW_INDEX][5] == EMPTY_PLACE
				&& position[FIRST_ROW_INDEX][6] == EMPTY_PLACE)
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
				&& position[FIRST_ROW_INDEX][3] == EMPTY_PLACE
				&& position[FIRST_ROW_INDEX][2] == EMPTY_PLACE
				&& position[FIRST_ROW_INDEX][1] == EMPTY_PLACE)
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

	MoveList ChessValidation::getValidRookMoves(const Position& position, INT32 x, INT32 y, short player)
	{
		MoveList moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<INT32, INT32>> directions = { {0,-1}, {1,0}, {0,1}, {-1,0} };
		
		for (auto direction : directions)
		{
			auto dirMoves = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), dirMoves.begin(), dirMoves.end());
		}

		return moves;
	}

	MoveList ChessValidation::getValidBishopMoves(const Position& position, INT32 x, INT32 y, short player)
	{
		MoveList moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<INT32, INT32>> directions = { {-1,-1}, {1,-1}, {1,1}, {-1,1} };

		for (auto direction : directions)
		{
			auto dirMoves = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), dirMoves.begin(), dirMoves.end());
		}

		return moves;
	}

	MoveList ChessValidation::continueValidMovesOnLine(const Position& position, INT32 x, INT32 y, const std::string& enemies_string, INT32 xDir, INT32 yDir)
	{
		MoveList moves;

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

				if (place == EMPTY_PLACE || capture)
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
		return player == PLAYER_WHITE ? BLACK_PIECES : WHITE_PIECES;
	}

	BOOL ChessValidation::isInsideChessboard(INT32 x, INT32 y)
	{
		return x>=FIRST_ROW_INDEX && x<COLUMNS && y>=FIRST_ROW_INDEX && y<ROWS;
	}
}