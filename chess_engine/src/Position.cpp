#include "Position.hpp"
#include <cassert>

namespace matt
{
	Position::Position()
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
}