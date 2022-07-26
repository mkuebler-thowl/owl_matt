#include "Position.hpp"
#include <cassert>

namespace matt
{
	Position::Position()
		: m_enPassant(false)
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

	std::array<char, 8>& Position::operator[](int index) const
	{
		assert(index < 8);
		return m_data[index];
	}
	void Position::setEnPassant(bool value)
	{
		m_enPassant = value;
	}
	bool Position::getEnPassant() const
	{
		return m_enPassant;
	}
}