#pragma once
#include "defines.hpp"
#include "Move.hpp"
#include <unordered_map>

namespace owl
{

	constexpr UINT16 VALUE = 0;
	constexpr UINT16 MIN_MAX_VALUE_SIZE = 1;

	using MoveMap = std::unordered_map<Move, FLOAT, Move::HashFunction>;

	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	class MinMaxResult
	{
	public:
		VOID insert(const Move& move, const FLOAT value) const;
		std::pair<Move, FLOAT> getResult() const;
		BOOL empty() const;
	private:
		void updateCurrentValue(FLOAT newValue) const;

		mutable FLOAT m_currentBestValue = -INF;
#if OWL_RANDOM==true
		mutable MoveMap m_result;
#else
		mutable Move m_best;
#endif
	};
}