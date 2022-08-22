#include "MinMaxResult.hpp"
#include <random>
#include <chrono>
#include <iostream>

namespace owl
{
	VOID MinMaxResult::insert(const Move& move, const EVALUATION_VALUE value, const MOVE_LIST* bestMoves) const
	{
		updateCurrentValue(value);
    // ZŸge ausgeben:
    #if LOG_MOVE_STACK
        m_movePath = *bestMoves;
    #endif
	#if OWL_USE_RANDOM==true
		m_result.emplace(move, value);
		
		// Lösche Züge, die kleiner sind als (Bester Zug-Threshold):
		for (auto it = m_result.begin(); it != m_result.end();)
		{
			if (it->second < m_currentBestValue - RANDOM_THRESHOLD)
			{
				it = m_result.erase(it);
			}
			else { it++; }
		}
	#else
		m_best = move;
	#endif
	}

	std::pair<Move, EVALUATION_VALUE> MinMaxResult::getResult() const
	{
	#if OWL_USE_RANDOM==true
		if (m_result.empty()) return { INVALID_MOVE, m_currentBestValue };

		// Wähle einen zufälligen Zug aus:
		std::default_random_engine generator;

		generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_int_distribution<INT64> distribution(0, m_result.size()-1);

		auto index = distribution(generator);
		return *std::next(std::begin(m_result), index);
	#else
		if (m_best.isMoveInvalid()) return { INVALID_MOVE , 0 };
		return { m_best, m_currentBestValue };
	#endif
	}

	BOOL MinMaxResult::empty() const
	{
	#if OWL_USE_RANDOM==true
		return m_result.empty();
	#else
		return m_best.isMoveInvalid();
	#endif
	}

	void MinMaxResult::updateCurrentValue(const EVALUATION_VALUE newValue) const
	{
		m_currentBestValue = newValue;
	}
}

