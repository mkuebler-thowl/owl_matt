#include "ChessValidation.hpp"
#include "ChessEngine.hpp"
#include "FENParser.hpp"

#include <string>
namespace matt
{
	std::vector<Move> ChessValidation::getValidMoves(const Position& position, short player, bool sort)
	{
		std::vector<Move> moves;

		int pawn_direction = player;

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (player == PLAYER_WHITE)
				{
					switch (position[y][x])
					{
					case 'P': {
						auto pawns = getValidPawnMoves(position, x, y, player);
						moves.insert(moves.end(), pawns.begin(), pawns.end());
						break; }
					case 'N': {
						auto knights = getValidKnightMoves(position, x, y, player);
						moves.insert(moves.end(), knights.begin(), knights.end());
						break; }
					case 'K': {
						auto kings = getValidKingMoves(position, x, y, player);
						moves.insert(moves.end(), kings.begin(), kings.end());
						break; }
					case 'R': {
						auto rooks = getValidRookMoves(position, x, y, player);
						moves.insert(moves.end(), rooks.begin(), rooks.end());
						break; }
					case 'B': {
						auto bishops = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), bishops.begin(), bishops.end());
						break; }
					case 'Q': {
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
					case 'p': {
						auto pawns = getValidPawnMoves(position, x, y, player);
						moves.insert(moves.end(), pawns.begin(), pawns.end());
						break; }
					case 'n': {
						auto knights = getValidKnightMoves(position, x, y, player);
						moves.insert(moves.end(), knights.begin(), knights.end());
						break; }
					case 'k': {
						auto kings = getValidKingMoves(position, x, y, player);
						moves.insert(moves.end(), kings.begin(), kings.end());
						break; }
					case 'r': {
						auto rooks = getValidRookMoves(position, x, y, player);
						moves.insert(moves.end(), rooks.begin(), rooks.end());
						break; }
					case 'b': {
						auto bishops = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), bishops.begin(), bishops.end());
						break; }
					case 'q': {
						auto axis = getValidRookMoves(position, x, y, player);
						auto diagonal = getValidBishopMoves(position, x, y, player);
						moves.insert(moves.end(), axis.begin(), axis.end());
						moves.insert(moves.end(), diagonal.begin(), diagonal.end());
						break; }
					}
				}
			}
		}
		return std::vector<Move>();
	}

	const Position& ChessValidation::applyMove(const Position& position, const Move& move)
	{
		auto pos = position;
		auto figure = position[move.startY][move.startX];

		pos[move.targetY][move.targetX] = figure;
		pos[move.startY][move.startX] = ' ';
		
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
		return pos;
	}

	bool ChessValidation::isKinginCheckAfterMove(const Position& position, short player, const Move& move)
	{
		auto nextPosition = applyMove(position, move);
		return isKingInCheck(nextPosition, player);
	}
	void ChessValidation::addAppliedMoveToRepitionMap(Position& position)
	{
		auto fen = FENParser::positionToFen(position);
		auto search = m_repitionMap.find(fen);

		if (search != m_repitionMap.end())
		{
			m_repitionMap[fen]++;
			// Stellungswiederholung 3x?
			if (m_repitionMap[fen] >= 3)
			{
				position.setGameState(GameState::Remis);
			}
		}
		else
		{
			m_repitionMap[fen] = 1;
		}
	}
	
	bool ChessValidation::isKingInCheck(const Position& position, short player)
	{
		char king = player == PLAYER_WHITE ? 'K' : 'k';
		char enemy_queen = player == PLAYER_WHITE ? 'q' : 'Q';
		char enemy_rook = player == PLAYER_WHITE ? 'r' : 'R';
		char enemy_bishop = player == PLAYER_WHITE ? 'b' : 'B';
		char enemy_knight = player == PLAYER_WHITE ? 'n' : 'N';
		char enemy_pawn = player == PLAYER_WHITE ? 'p' : 'P';

		int king_x, king_y;
		bool king_found = false;

		// König suchen
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (position[y][x] == king)
				{
					king_x = x;
					king_y = y;
					y = 8;
					king_found = true;
					break;
				}
			}
		}

		// Wenn König nicht gefunden:
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

		// Springerfelder im Uhrzeigersinn (1-8) überprüfen:
		// Springerfeld 1
		if (king_x < 7 && king_y > 1)
		{
			if (position[king_y - 2][king_x + 1] == enemy_knight) return true;
		}
		// Springerfeld 2
		if (king_x < 6 && king_y > 0)
		{
			if (position[king_y - 1][king_x + 2] == enemy_knight) return true;
		}
		// Springerfeld 3
		if (king_x < 6 && king_y < 7)
		{
			if (position[king_y + 1][king_x + 2] == enemy_knight) return true;
		}
		// Springerfeld 4
		if (king_x < 7 && king_y < 6)
		{
			if (position[king_y + 2][king_x + 1] == enemy_knight) return true;
		}
		// Springerfeld 5
		if (king_x > 0 && king_y < 6)
		{
			if (position[king_y + 2][king_x - 1] == enemy_knight) return true;
		}
		// Springerfeld 6
		if (king_x > 1 && king_y < 7)
		{
			if (position[king_y + 1][king_x - 2] == enemy_knight) return true;
		}
		// Springerfeld 7
		if (king_x > 1 && king_y > 0)
		{
			if (position[king_y - 1][king_x - 2] == enemy_knight) return true;
		}
		// Springerfeld 8
		if (king_x > 0 && king_y > 1)
		{
			if (position[king_y - 2][king_x - 1] == enemy_knight) return true;
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

	std::vector<Move> ChessValidation::getValidPawnMoves(const Position& position, int x, int y, short player)
	{
		std::vector<Move> moves;

		auto pawn_direction = player;
		std::string enemies = player == PLAYER_WHITE ? "pnbrqk" : "PNBRQK";
		std::string promotion_str = player == PLAYER_WHITE ? "NBRQ" : "nbrq";

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
		std::vector<std::pair<int, int>> possible_places = { {1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1} };
		std::string enemies = player == PLAYER_WHITE ? "pnbrqk" : "PNBRQK";

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
		std::string enemies = player == PLAYER_WHITE ? "pnbrqk" : "PNBRQK";
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
		return x>=0 && x<8 && y>=0 && y<8;
	}
}