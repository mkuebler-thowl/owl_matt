#pragma once
#include "defines.hpp"
#include "Move.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace owl
{
	// TODO: Für random eine Liste benutzen mit best moves? 

	constexpr UINT16 VALUE = 0;

	constexpr UINT16 ALPHA = 1;
	constexpr UINT16 BETA = 2;

	constexpr UINT16 LOWER = 1;
	constexpr UINT16 UPPER = 2;
	constexpr UINT16 NESTED_ALPHA = 3;
	constexpr UINT16 NESTED_BETA = 4;

	constexpr UINT16 MIN_MAX_VALUE_SIZE = 1;
	constexpr UINT16 ALPHA_BETA_SIZE = 3;

	constexpr UINT16 NESTED_SIZE = 3;
	constexpr UINT16 NESTED_ALPHA_BETA_SIZE = 5;

	using MoveSet = std::unordered_set<Move, Move::HashFunction>;

	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	struct MinMaxResult
	{
		Move best;
		std::vector<FLOAT> values;
	};
}