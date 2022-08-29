#include "MinMaxResult.hpp"
#include <random>
#include <chrono>
#include <iostream>

namespace owl
{
	VOID MinMaxResult::insert(const Move& move, const EVALUATION_VALUE value, BOOL shouldMax) const
	{
		updateCurrentValue(value, shouldMax);
	#if OWL_USE_RANDOM==true
		if (m_result.size() < MAX_RANDOM_MOVES_SIZE)
		{
			m_result.emplace(move, value);
		}
		else
		{
			auto min = m_result.begin();
			auto max = m_result.begin();

			auto min_value = INF;
			auto max_value = -INF;

			for (auto it = m_result.begin(); it != m_result.end(); it++)
			{
				if (it->second < min_value) 
				{
					min_value = it->second;
					min = it;
				}
				if (it->second > max_value)
				{
					max_value = it->second;
					max = it;
				}
			}

			if ((value > min_value) && value > (max_value - RANDOM_THRESHOLD))
			{
				m_result.erase(min);
				m_result.emplace(move, value);
			}
		}

		// Lösche Züge, die kleiner sind als (Bester Zug-Threshold):
		//for (auto it = m_result.begin(); it != m_result.end();)
		//{
		//	if (it->second < static_cast<FLOAT>(m_currentBestValue) - RANDOM_THRESHOLD || it->second > m_currentBestValue)
		//	{
		//		it = m_result.erase(it);
		//	}
		//	else { it++; }
		//}

		//// Lösche Züge, wenn die Anzahl der maximal gültigen Zufallszüge überschritten wurde
		//if (m_result.size() > MAX_RANDOM_MOVES_SIZE)
		//{
		//	auto worst_value = m_currentBestValue;
		//	auto worst_it = m_result.begin();

		//	for (auto it = m_result.begin(); it != m_result.end(); it++)
		//	{
		//		if (it->second < worst_value)
		//		{
		//			worst_value = it->second;
		//			worst_it = it;
		//		}
		//	}

		//	m_result.erase(worst_it);
		//}
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

	VOID MinMaxResult::clear() const
	{
#if OWL_USE_RANDOM==true
		m_result.clear();
#endif
		m_currentBestValue = -INF;
	}

	VOID MinMaxResult::updateCurrentValue(const EVALUATION_VALUE newValue, BOOL shouldMax) const
	{
		if (shouldMax)
		{
			m_currentBestValue = std::max(m_currentBestValue, newValue);
		}
		else {
			m_currentBestValue = std::min(m_currentBestValue, newValue);
		}
	}
#if OWL_USE_RANDOM==true
	const MoveMap MinMaxResult::testGetResults() const
	{
		return m_result;
	}
#endif
}

