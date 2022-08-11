#pragma once

#include "Move.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace owl
{
	// TODO: Für random eine Liste benutzen mit best moves? 

	constexpr unsigned short VALUE = 0;

	constexpr unsigned short ALPHA = 1;
	constexpr unsigned short BETA = 2;

	constexpr unsigned short LOWER = 1;
	constexpr unsigned short UPPER = 2;
	constexpr unsigned short NESTED_ALPHA = 3;
	constexpr unsigned short NESTED_BETA = 4;

	constexpr unsigned short MIN_MAX_VALUE_SIZE = 1;
	constexpr unsigned short ALPHA_BETA_SIZE = 3;

	constexpr unsigned short NESTED_SIZE = 3;
	constexpr unsigned short NESTED_ALPHA_BETA_SIZE = 5;

	using MoveSet = std::unordered_set<Move, Move::HashFunction>;

	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	struct MinMaxResult
	{
		Move best;
		std::vector<float> values;
		std::unordered_map<unsigned short, MoveSet> killers;
	};
}