#pragma once

#include "Move.hpp"
#include <vector>

namespace matt
{
	// TODO: Für random eine Liste benutzen mit best moves? 

	constexpr unsigned short VALUE = 0;

	constexpr unsigned short ALPHA = 1;
	constexpr unsigned short BETA = 2;

	constexpr unsigned short MIN_MAX_VALUE_SIZE = 1;
	constexpr unsigned short ALPHA_BETA_SIZE = 3;

	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	struct MinMaxResult
	{
		Move best;
		std::vector<float> values;
	};
}