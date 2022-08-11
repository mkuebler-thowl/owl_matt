#include "RepitionMap.hpp"
#include "FENParser.hpp"


namespace owl
{
	void RepitionMap::addPosition(const Position& position)
	{
		auto search = m_data.find(position);

		if (search != m_data.end())
		{
			m_data[position]++;
			// Stellungswiederholung 3x?
			if (m_data[position] >= 3)
			{
				position.setGameState(GameState::Remis);
			}
		}
		else
		{
			m_data[position] = 1;
		}
	}
	bool RepitionMap::isPositionAlreadyLocked(const Position& position) const
	{
		auto search = m_data.find(position);
		if (search != m_data.end())
		{
			if (m_data[position] >= 2) return true;
		}
		
		return false;
	}
}