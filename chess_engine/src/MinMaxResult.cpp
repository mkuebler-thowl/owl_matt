#include "MinMaxResult.hpp"
#include <random>
#include <chrono>
#include <iostream>

namespace owl
{
	VOID MinMaxResult::insert(const Move& move, const FLOAT value) const
	{
		updateCurrentValue(value);
	#if OWL_RANDOM==true
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

	std::pair<Move, FLOAT> MinMaxResult::getResult() const
	{
	#if OWL_RANDOM==true
		if (m_result.empty()) return { INVALID_MOVE, m_currentBestValue };

		// Wähle einen zufälligen Zug aus:
		std::default_random_engine generator;

		generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

		std::uniform_int_distribution<INT64> distribution(0, m_result.size()-1);

		auto index = distribution(generator);
		return *std::next(std::begin(m_result), index);
	#else
		return { m_best, m_currentBestValue };
	#endif
	}

	BOOL MinMaxResult::empty() const
	{
	#if OWL_RANDOM==true
		return m_result.empty();
	#else
		return m_best.isMoveInvalid();
	#endif
	}

	void MinMaxResult::updateCurrentValue(FLOAT newValue) const
	{
		m_currentBestValue = newValue;
	}
}