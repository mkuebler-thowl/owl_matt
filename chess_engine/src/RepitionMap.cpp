#include "RepitionMap.hpp"
#include "FENParser.hpp"


namespace matt
{
	void RepitionMap::addAppliedMoveToRepitionMap(Position& position)
	{
		auto fen = FENParser::positionToFen(position);
		auto search = m_data.find(fen);

		if (search != m_data.end())
		{
			m_data[fen]++;
			// Stellungswiederholung 3x?
			if (m_data[fen] >= 3)
			{
				position.setGameState(GameState::Remis);
			}
		}
		else
		{
			m_data[fen] = 1;
		}
	}
}