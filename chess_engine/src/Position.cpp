#include "Position.hpp"
#include <cassert>
#include "ChessEngine.hpp"
#include <iostream>

namespace owl
{
	Position::Position()
		: m_enPassant(false), m_movesCount(0), m_moveNumber(1), m_player(PLAYER_WHITE), m_state(GameState::Active), 
		m_whiteCastlingShort(true), m_whiteCastlingLong(true), m_blackCastlingShort(true), m_blackCastlingLong(true),
		m_gamePhase(GamePhase::Opening)
	{
		// 2D Array mit 'whitespace' initialisieren
		for (auto i = 0; i < 8; i++)
		{
			for (auto j = 0; j < 8; j++)
			{
				m_data[i][j] = ' ';
			}
		}
	}

	Position::~Position()
	{

	}

	Position::Position(const BoardArray& data, short player,
		bool whiteCastlingShort, bool whiteCastlingLong, 
		bool blackCastlingShort, bool blackCastlingLong, 
		bool enPassant, std::pair<int, int> enPassantPosition, 
		int moveCount, int moveNumber)
		: m_data(data), m_player(player), 
		m_whiteCastlingShort(whiteCastlingShort), m_whiteCastlingLong(whiteCastlingLong),
		m_blackCastlingShort(blackCastlingShort), m_blackCastlingLong(blackCastlingLong),
		m_enPassant(enPassant), m_enPassantPosition(enPassantPosition), 
		m_movesCount(moveCount), m_moveNumber(moveNumber), m_gamePhase(GamePhase::Opening), m_state(GameState::Active)
	{

	}

	owl::BoardLine& Position::operator[](int index) const
	{
		assert(index < COLUMNS);
		return m_data[index];
	}
	void Position::setEnPassant(int x, int y)
	{
		m_enPassant = true;
		m_enPassantPosition = { x,y };
	}

	void Position::resetEnPassant()
	{
		m_enPassant = false;
	}

	bool Position::isEnPassant() const
	{
		return m_enPassant;
	}
	const std::pair<int, int>& Position::getEnPassant() const
	{
		return m_enPassantPosition;
	}
	void Position::addMoveCount()
	{
		m_movesCount++;
	}
	void Position::resetMoveCount()
	{
		m_movesCount = 0;
	}
	int Position::getMoveCount() const
	{
		return m_movesCount;
	}
	int Position::getMoveNumber() const
	{
		return m_moveNumber;
	}
	void Position::changePlayer()
	{
		// Zugnummer erhöhen
		if (m_player == PLAYER_BLACK) m_moveNumber++;
		// Spieler wechseln
		m_player = m_player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	}
	int Position::getPlayer() const
	{
		return m_player;
	}
	void Position::setGameState(GameState state) const
	{
		m_state = state;
	}

	GameState Position::getGameState() const
	{
		return m_state;
	}
	bool Position::getWhiteCastlingShort() const
	{
		return m_whiteCastlingShort;
	}
	bool Position::getWhiteCastlingLong() const
	{
		return m_whiteCastlingLong;
	}
	bool Position::getBlackCastlingShort() const
	{
		return m_blackCastlingShort;
	}
	bool Position::getBlackCastlingLong() const
	{
		return m_blackCastlingLong;
	}
	void Position::resetWhiteCastlingShort()
	{
		m_whiteCastlingShort = false;
	}
	void Position::resetWhiteCastlingLong()
	{
		m_whiteCastlingLong = false;
	}
	void Position::resetBlackCastlingShort()
	{
		m_blackCastlingShort = false;
	}
	void Position::resetBlackCastlingLong()
	{
		m_blackCastlingLong = false;
	}
	void Position::enterNextGamePhase() const
	{
		if (m_gamePhase == GamePhase::Mid) m_gamePhase = GamePhase::End;
		else if (m_gamePhase == GamePhase::Opening) m_gamePhase = GamePhase::Mid;
	}

	GamePhase Position::getGamePhase() const
	{
		return m_gamePhase;
	}
	void Position::printPosition(const Position& position)
	{
		std::string out = "";

		for (auto i = 0; i < ROWS; i++)
		{
			for (auto j = 0; j < COLUMNS; j++)
			{
				out += position[i][j];
			}
			out += "\n";
		}

		std::cout << out;
	}
}