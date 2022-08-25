#pragma once
#include "defines.hpp"
#include "Move.hpp"
#include <unordered_map>

namespace owl
{
	constexpr INT32 VALUE = 0;
	constexpr INT32 MIN_MAX_VALUE_SIZE = 1;

	using MoveMap = std::unordered_map<Move, EVALUATION_VALUE, Move::HashFunction>;

	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	class MinMaxResult
	{
	public:
		VOID insert(const Move& move, const EVALUATION_VALUE value, BOOL shouldMax) const;
		std::pair<Move, EVALUATION_VALUE> getResult() const;
		BOOL empty() const;
		
	private:
		void updateCurrentValue(const EVALUATION_VALUE newValue, BOOL shouldMax) const;
		mutable EVALUATION_VALUE m_currentBestValue = -INF;
#if OWL_USE_RANDOM==true
	private:
		mutable MoveMap m_result;
	public:
		const MoveMap testGetResults() const;
#else
		mutable Move m_best;
#endif
	};
}
