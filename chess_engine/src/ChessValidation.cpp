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

		for (int y = 0; y < ROWS; y++)
		{
			for (int x = 0; x < COLUMNS; x++)
			{
				if (player == PLAYER_WHITE)
				{
					switch (position[y][x])
					{
					case PAWN_WHITE: {
						auto pawns = getValidPawnMoves(position, x, y, player);
						moves.insert(moves.end(), pawns.begin(), pawns.end());
						break; }
					case KNIGHT_WHITE: {
						auto knights = getValidKnightMoves(position, x, y, player);
						moves.insert(moves.end(), knights.begin(), knights.end());
						break; }
					case KING_WHITE: {
						auto kings = getValidKingMoves(position, x, y, player);
						moves.insert(moves.end(), kings.begin(), kings.end());
						break; }
					case ROOK_WHITE: {
						auto rooks = getValidRookMoves(position, x, y, player);
						moves.insert(moves.end(), rooks.begin(), rooks.end());
						break; }
					case BISHOP_WHITE: {
						auto bishops = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), bishops.begin(), bishops.end());
						break; }
					case QUEEN_WHITE: {
						auto axis = getValidRookMoves(position, x, y, player);
						auto diagonal = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), axis.begin(), axis.end());
						moves.insert(moves.end(), diagonal.begin(), diagonal.end());
						break; }
					}
				}
				else if (player == PLAYER_BLACK)
				{
					switch (position[y][x])
					{
					case PAWN_BLACK: {
						auto pawns = getValidPawnMoves(position, x, y, player);
						moves.insert(moves.end(), pawns.begin(), pawns.end());
						break; }
					case KNIGHT_BLACK: {
						auto knights = getValidKnightMoves(position, x, y, player);
						moves.insert(moves.end(), knights.begin(), knights.end());
						break; }
					case KING_BLACK: {
						auto kings = getValidKingMoves(position, x, y, player);
						moves.insert(moves.end(), kings.begin(), kings.end());
						break; }
					case ROOK_BLACK: {
						auto rooks = getValidRookMoves(position, x, y, player);
						moves.insert(moves.end(), rooks.begin(), rooks.end());
						break; }
					case BISHOP_BLACK: {
						auto bishops = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), bishops.begin(), bishops.end());
						break; }
					case QUEEN_BLACK: {
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
	const Position& ChessValidation::applyMove(const Position& position, const Move& move)
	{
		auto pos = position;
		auto figure = position[move.startY][move.startX];

		pos[move.targetY][move.targetX] = figure;
		pos[move.startY][move.startX] = ' ';
		
		// Ist der Zug eine Rochade? => Turm bewegen
		if (move.castlingLong)
		{
			auto rook = position[move.startY][0];
			pos[move.startY][0] = ' ';
			pos[move.startY][3] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = position[move.startY][COLUMNS-1];
			pos[move.startY][COLUMNS-1] = ' ';
			pos[move.startY][5] = rook;
		}

		// Wurde die En Passant-Capture eingelöst? Lösche richtiges Feld
		if (move.enPassantCapture && (move.targetY == 2 || move.targetY == 5)) 
		{
			pos[move.startY][move.targetX] = ' ';
		}

		pos.addMoveCount();
		pos.changePlayer();

		// En Passant zurücksetzen:
		if (position.isEnPassant())
		{
			pos.resetEnPassant();
		}

		// 50-Züge-Regel: Capture abfragen & Bauernbewegung
		if (figure == 'P' || figure == 'p' || move.capture)
		{
			pos.resetMoveCount();
		}

		// En passant für die Folgeposition bestimmen:
		if ((figure == 'p' && move.startY == 1 && move.targetY == 3)
			|| (figure == 'P' && move.startY == 6 && move.targetY == 4))
		{
			auto dir = move.startY == 1 ? 1 : -1;
			pos.setEnPassant(move.startX, move.startY + dir);
		}

		// 100 Halbzüge woanders informieren; auswerten
		if (pos.getMoveCount() >= 100) // 100 Halbzüge bzw. 50 Züge
		{
			pos.setGameState(GameState::Remis);
		}

		// Rochieren gegebenfalls deaktivieren
		if (pos.getWhiteCastlingShort() || pos.getWhiteCastlingLong() || pos.getBlackCastlingLong() || pos.getBlackCastlingShort())
		{
			switch (figure)
			{
			case 'K':
				pos.resetWhiteCastlingLong();
				pos.resetWhiteCastlingShort();
				break;
			case 'k':
				pos.resetBlackCastlingLong();
				pos.resetBlackCastlingShort();
				break;
			case 'R':
				if (move.startX == 0) pos.resetWhiteCastlingLong();
				else if (move.startX == 7) pos.resetWhiteCastlingShort();
				break;
			case 'r':
				if (move.startX == 0) pos.resetBlackCastlingLong();
				else if (move.startX == 7) pos.resetBlackCastlingShort();
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
		char king = player == PLAYER_WHITE ? 'K' : 'k';
		int king_x, king_y;
		bool king_found = false;

		// König suchen
		for (int y = 0; y < ROWS; y++)
		{
			for (int x = 0; x < COLUMNS; x++)
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
		// links vom König:
		if (king_x > 0)
		{
			for (int x = king_x - 1; x >= 0; x--)
			{
				auto place = position[king_y][x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// rechts vom König:
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
		// oben vom König:
		if (king_y > 0)
		{
			for (int y = king_y - 1; y >= 0; y--)
			{
				auto place = position[y][king_x];
				if (place == enemy_queen || place == enemy_rook) return true;
				if (place != ' ') break;
			}
		}
		// unten vom König:
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
		char enemy_pawn = player == PLAYER_WHITE ? 'p' : 'P';

		// Weißen König auf schwarze Bauern überprüfen:
		//  p p
		//   K
		if (player == PLAYER_WHITE)
		{
			// links oben:
			if (king_y > 0 && king_x > 0)
			{
				auto piece = position[king_y - 1][king_x - 1];
				if (piece == enemy_pawn) return true;
			}
			// rechts oben:
			if (king_y > 0 && king_x < 7)
			{
				auto piece = position[king_y - 1][king_x + 1];
				if (piece == enemy_pawn) return true;
			}
		}
		else if (player == PLAYER_BLACK)
		{
			// links unten:
			if (king_y < 7 && king_x > 0)
			{
				auto piece = position[king_y + 1][king_x - 1];
				if (piece == enemy_pawn) return true;
			}
			// rechts unten:
			if (king_y < 7 && king_x < 7)
			{
				auto piece = position[king_y + 1][king_x + 1];
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
			// Pattsituation (Spieler kann keine Bewegung durchführen)
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
		std::string enemies = player == PLAYER_WHITE ? BLACK_PIECES : WHITE_PIECES;
		std::string promotion_str = player == PLAYER_WHITE ? WHITE_PROMOTION_STR : BLACK_PROMOTION_STR;

		if (y > 0 && player == PLAYER_WHITE || y < 7 && player == PLAYER_BLACK)
		{
			// Schritt nach vorne 
			if (position[y - pawn_direction][x] == ' ')
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
			if ((y == 6 && player == PLAYER_WHITE && position[4][x] == ' ' && position[5][x] == ' ')
				|| (y == 1 && player == PLAYER_BLACK && position[2][x] == ' ' && position[3][x] == ' '))
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
			if (x > 0)
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

	std::vector<Move> ChessValidation::getValidKnightMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;

		std::vector<std::pair<int, int>> possible_places = { {1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1}, {-1,-2} };
		std::string enemies = player == PLAYER_WHITE ? BLACK_PIECES : WHITE_PIECES;

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

	std::vector<Move> ChessValidation::getValidKingMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;
		std::string enemies = player == PLAYER_WHITE ? BLACK_PIECES : WHITE_PIECES;
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
				&& isPlaceInCheck(position, 5, ROWS - 1, player) 
				&& isPlaceInCheck(position, 6, ROWS - 1, player)
				&& position[7][5] == ' '
				&& position[7][6] == ' ')
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
				&& isPlaceInCheck(position, 2, ROWS - 1, player)
				&& isPlaceInCheck(position, 3, ROWS - 1, player)
				&& position[7][3] == ' '
				&& position[7][2] == ' '
				&& position[7][1] == ' ')
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
				&& isPlaceInCheck(position, 5, 0, player)
				&& isPlaceInCheck(position, 6, 0, player)
				&& position[0][5] == ' '
				&& position[0][6] == ' ')
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
				&& isPlaceInCheck(position, 2, 0, player)
				&& isPlaceInCheck(position, 3, 0, player)
				&& position[0][3] == ' '
				&& position[0][2] == ' '
				&& position[0][1] == ' ')
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
		std::string enemies = player == PLAYER_WHITE ? "pnbrqk" : "PNBRQK";
		std::vector<std::pair<int, int>> directions = { {0,-1}, {1,0}, {0,-1}, {-1,0} };
		
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
		std::string enemies = player == PLAYER_WHITE ? "pnbrqk" : "PNBRQK";
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
				auto place = position[line_x][line_y];
				auto capture = enemies_string.find(place) != std::string::npos;

				if (place == ' ' || capture)
				{
					Move move;
					move.startX = x;
					move.startY = y;
					move.targetX = line_x;
					move.targetY = line_y;
					move.capture = capture;
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

	bool ChessValidation::isInsideChessboard(int x, int y)
	{
		return x>=0 && x<COLUMNS && y>=0 && y<ROWS;
	}
}