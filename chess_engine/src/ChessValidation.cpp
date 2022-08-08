#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"
#include "ChessEvaluation.hpp"

#include <string>
#include <iostream>
namespace matt
{
	std::vector<Move> ChessValidation::getValidMoves(const Position& position, short player, bool sort)
	{
		std::vector<Move> moves;

		for (int y = FIRSTROWINDEX; y < ROWS; y++)
		{
			for (int x = FIRSTCOLINDEX; x < COLUMNS; x++)
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
		// Da jeder Zug, der ein "Schach" ausl�st entfernt wird, gilt das nat�rlich auch f�r alle Folgez�ge, die eine "Schach"-Situation nicht aufl�sen k�nnen.
		// Deshalb reicht es abzufragen, ob der K�nig im Schach steht und keine Z�ge mehr zur Verf�gung stehen
		evaluateCheckmate(position, player, moves.empty());

		return moves;
	}
	unsigned short ChessValidation::countPossibleMovesOnField(const Position& position, int x, int y)
	{
		unsigned short count = 0;

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
			auto rook = position[move.startY][FIRSTCOLINDEX];
			pos[move.startY][FIRSTCOLINDEX] = EMPTY_PLACE;
			pos[move.startY][3] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = position[move.startY][COLUMNS-1];
			pos[move.startY][LASTCOLINDEX] = EMPTY_PLACE;
			pos[move.startY][5] = rook;
		}

		// Wurde die En Passant-Capture eingel�st? L�sche richtiges Feld
		if (move.enPassantCapture && (move.targetY == 2 || move.targetY == 5)) 
		{
			pos[move.startY][move.targetX] = EMPTY_PLACE;
		}

		pos.addMoveCount();
		pos.changePlayer();

		// En Passant zur�cksetzen:
		if (position.isEnPassant())
		{
			pos.resetEnPassant();
		}

		// 50-Z�ge-Regel: Capture abfragen & Bauernbewegung
		if (figure == PAWN_WHITE || figure == PAWN_BLACK || move.capture)
		{
			pos.resetMoveCount();
		}

		// En passant f�r die Folgeposition bestimmen:
		if ((figure == PAWN_BLACK && move.startY == 1 && move.targetY == 3)
			|| (figure == PAWN_WHITE && move.startY == 6 && move.targetY == 4))
		{
			auto dir = move.startY == 1 ? 1 : -1;
			pos.setEnPassant(move.startX, move.startY + dir);
		}

		// 100 Halbz�ge woanders informieren; auswerten
		if (pos.getMoveCount() >= 100) // 100 Halbz�ge bzw. 50 Z�ge
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
				if (move.startX == FIRSTCOLINDEX) pos.resetWhiteCastlingLong();
				else if (move.startX == LASTCOLINDEX) pos.resetWhiteCastlingShort();
				break;
			case ROOK_BLACK:
				if (move.startX == FIRSTCOLINDEX) pos.resetBlackCastlingLong();
				else if (move.startX == LASTCOLINDEX) pos.resetBlackCastlingShort();
			}
		}

		return pos;
	}

	bool ChessValidation::isKinginCheckAfterMove(const Position& position, short player, const Move& move)
	{
		auto nextPosition = applyMove(position, move);
		return isKingInCheck(nextPosition, player);
	}
	bool ChessValidation::isKingInCheck(const Position& position, short player)
	{
		char king = PIECES[ChessEvaluation::GetPlayerIndexByPositionPlayer(player)][KING];
		int king_x, king_y;
		bool king_found = false;

		// K�nig suchen
		for (int y = FIRSTROWINDEX; y < ROWS; y++)
		{
			for (int x = FIRSTCOLINDEX; x < COLUMNS; x++)
			{
				if (position[y][x] == king)
				{
					king_x = x;
					king_y = y;
					y = ROWS; // Grenze �berschreiten (outer-break)
					king_found = true;
					break;
				}
			}
		}

		// Falls der K�nig nicht gefunden wird:
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
	bool ChessValidation::isPlaceInCheck(const Position& position, int x, int y, short player)
	{
		if (checkKingPawns(x, y, position, player)) return true;
		if (checkKingAxis(x, y, position, player)) return true;
		if (checkKingDiagonal(x, y, position, player)) return true;
		if (checkKingKnights(x, y, position, player)) return true;

		return false;
	}

	bool ChessValidation::checkKingDiagonal(int king_x, int king_y, const Position& position, short player)
	{
		char enemy_queen = player == PLAYER_WHITE ? 'q' : 'Q';
		char enemy_bishop = player == PLAYER_WHITE ? 'b' : 'B';

		// diagonal links oben:
		if (king_x > 0 && king_y > 0)
		{
			for (int i = std::min(king_x, king_y); i >= 0; i--)
			{
				auto place = position[king_y - i][king_x - i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}
		// diagonal rechts unten:
		if (king_x < 7 && king_y < 7)
		{
			for (int i = 1; i <= std::min(7 - king_x, 7 - king_y); i++)
			{
				auto place = position[king_y + i][king_x + i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}
		// diagonal rechts oben:
		if (king_x < 7 && king_y > 0)
		{
			for (int i = 1; i <= std::min(king_y, 7 - king_x); i++)
			{
				auto place = position[king_y - i][king_x + i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}
		// diagonal links unten:
		if (king_x > 0 && king_y < 7)
		{
			for (int i = 1; i <= std::min(7 - king_y, king_x); i++)
			{
				auto place = position[king_y + i][king_x - i];
				if (place == enemy_queen || place == enemy_bishop) return true;
				if (place != ' ') break;
			}
		}

		return false;
	}
	bool ChessValidation::checkKingAxis(int king_x, int king_y, const Position& position, short player)
	{
		char enemy_queen = player == PLAYER_WHITE ? 'q' : 'Q';
		char enemy_rook = player == PLAYER_WHITE ? 'r' : 'R';

		// horizontal:
		// links vom K�nig:
		if (king_x > 0)
		{
			for (int x = king_x - 1; x >= 0; x--)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// rechts vom K�nig:
		if (king_x < 7)
		{
			for (int x = king_x + 1; x <= 7; x++)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}

		// vertikal:
		// oben vom K�nig:
		if (king_y > 0)
		{
			for (int y = king_y - 1; y >= 0; y--)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// unten vom K�nig:
		if (king_y < 7)
		{
			for (int y = king_y + 1; y <= 7; y++)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		return false;
	}
	bool ChessValidation::checkKingKnights(int king_x, int king_y, const Position& position, short player)
	{
		char enemy_knight = player == PLAYER_WHITE ? 'n' : 'N';
		std::vector<std::pair<int, int>> directions = { {-1,-2}, {2,-1}, {2,1}, {1,2}, {-1,2}, {-2,1}, {-2,-1}, {-1,-2} };

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
	bool ChessValidation::checkKingPawns(int king_x, int king_y, const Position& position, short player)
	{
		char enemy_pawn = ChessEvaluation::GetEnemyPiece(player, PAWN);
		constexpr short king_offset = 1;

		// Wei�en K�nig auf schwarze Bauern �berpr�fen:
		//  p p
		//   K
		if (player == PLAYER_WHITE)
		{
			// links oben:
			if (king_y > FIRSTROWINDEX && king_x > FIRSTCOLINDEX)
			{
				auto piece = position[king_y - king_offset][king_x - king_offset];
				if (piece == enemy_pawn) return true;
			}
			// rechts oben:
			if (king_y > FIRSTROWINDEX && king_x < LASTCOLINDEX)
			{
				auto piece = position[king_y - king_offset][king_x + king_offset];
				if (piece == enemy_pawn) return true;
			}
		}
		else if (player == PLAYER_BLACK)
		{
			// links unten:
			if (king_y < LASTROWINDEX && king_x > FIRSTCOLINDEX)
			{
				auto piece = position[king_y + king_offset][king_x - king_offset];
				if (piece == enemy_pawn) return true;
			}
			// rechts unten:
			if (king_y < LASTROWINDEX && king_x < FIRSTCOLINDEX)
			{
				auto piece = position[king_y + king_offset][king_x + king_offset];
				if (piece == enemy_pawn) return true;
			}
		}

		return false;
	}
	void ChessValidation::evaluateCheckmate(const Position& position, short player, bool noValidMoves)
	{
		if (noValidMoves)
		{
			// Sieg & Niederlage
			if (isKingInCheck(position, player))
			{
				auto state = player == PLAYER_WHITE ? GameState::PlayerBlackWins : GameState::PlayerWhiteWins;
				position.setGameState(state);
			}
			// Pattsituation (Spieler kann keine Bewegung durchf�hren)
			else
			{
				position.setGameState(GameState::Remis);
			}
		}
	}

	std::vector<Move> ChessValidation::getValidPawnMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;

		auto pawn_direction = player;
		std::string enemies = getEnemyPiecesString(player);
		std::string promotion_str = player == PLAYER_WHITE ? WHITE_PROMOTION_STR : BLACK_PROMOTION_STR;

		if (y > FIRSTROWINDEX && player == PLAYER_WHITE || y < LASTROWINDEX && player == PLAYER_BLACK)
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
			if (x > FIRSTCOLINDEX)
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

				// En Passant �berpr�fen
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

	std::vector<Move> ChessValidation::getValidKnightMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;

		std::vector<std::pair<int, int>> possible_places = { {1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1}, {-1,-2} };
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

					// Pr�fen ob der K�nig nach dem Zug nicht im Schach steht:
					if(!isKinginCheckAfterMove(position, player, move))
						moves.push_back(move);
				}
			}
		}
		return moves;
	}

	std::vector<Move> ChessValidation::getValidKingMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<int, int>> possible_places = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1} };

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

					// Pr�fen ob der K�nig nach dem Zug nicht im Schach steht:
					if (!isKinginCheckAfterMove(position, player, move))
						moves.push_back(move);
				}
			}
		}

		// Rochade 
		if (player == PLAYER_WHITE)
		{
			if (position.getWhiteCastlingShort() 
				&& isPlaceInCheck(position, 5, LASTROWINDEX, player)
				&& isPlaceInCheck(position, 6, LASTROWINDEX, player)
				&& position[LASTROWINDEX][5] == EMPTY_PLACE
				&& position[LASTROWINDEX][6] == EMPTY_PLACE)
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
				&& isPlaceInCheck(position, 2, LASTROWINDEX, player)
				&& isPlaceInCheck(position, 3, LASTROWINDEX, player)
				&& position[LASTROWINDEX][3] == EMPTY_PLACE
				&& position[LASTROWINDEX][2] == EMPTY_PLACE
				&& position[LASTROWINDEX][1] == EMPTY_PLACE)
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
				&& isPlaceInCheck(position, 5, FIRSTROWINDEX, player)
				&& isPlaceInCheck(position, 6, FIRSTROWINDEX, player)
				&& position[FIRSTROWINDEX][5] == EMPTY_PLACE
				&& position[FIRSTROWINDEX][6] == EMPTY_PLACE)
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
				&& isPlaceInCheck(position, 2, FIRSTROWINDEX, player)
				&& isPlaceInCheck(position, 3, FIRSTROWINDEX, player)
				&& position[FIRSTROWINDEX][3] == EMPTY_PLACE
				&& position[FIRSTROWINDEX][2] == EMPTY_PLACE
				&& position[FIRSTROWINDEX][1] == EMPTY_PLACE)
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

	std::vector<Move> ChessValidation::getValidRookMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<int, int>> directions = { {0,-1}, {1,0}, {0,1}, {-1,0} };
		
		for (auto direction : directions)
		{
			auto dirMoves = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), dirMoves.begin(), dirMoves.end());
		}

		return moves;
	}

	std::vector<Move> ChessValidation::getValidBishopMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;
		std::string enemies = getEnemyPiecesString(player);
		std::vector<std::pair<int, int>> directions = { {-1,-1}, {1,-1}, {1,1}, {-1,1} };

		for (auto direction : directions)
		{
			auto dirMoves = continueValidMovesOnLine(position, x, y, enemies, direction.first, direction.second);
			moves.insert(moves.end(), dirMoves.begin(), dirMoves.end());
		}

		return moves;
	}

	std::vector<Move> ChessValidation::continueValidMovesOnLine(const Position& position, int x, int y, const std::string& enemies_string, int xDir, int yDir)
	{
		std::vector<Move> moves;

		bool line_empty = true;

		int offset = 1;

		while (line_empty)
		{
			int line_x = x + xDir * offset;
			int line_y = y + yDir * offset;

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

	bool ChessValidation::isInsideChessboard(int x, int y)
	{
		return x>=FIRSTROWINDEX && x<COLUMNS && y>=FIRSTROWINDEX && y<ROWS;
	}
}