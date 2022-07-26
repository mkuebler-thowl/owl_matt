#include "ChessValidation.hpp"
#include "ChessEngine.hpp"

namespace matt
{
	std::vector<Move> ChessValidation::getValidMoves(const Position& position, short player, bool sort)
	{
		std::vector<Move> moves;

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (player == PLAYER_WHITE)
				{
					switch (position[y][x])
					{
					case 'P':
						// TODO:
						break;
					}
				}
			}
		}
		return std::vector<Move>();
	}

	const Position& ChessValidation::applyMove(const Position& position, const Move& move)
	{
		auto pos = position;
		pos[move.targetY][move.targetX] = position[move.startY][move.startX];
		pos[move.startY][move.startX] = ' ';
		return pos;
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
}