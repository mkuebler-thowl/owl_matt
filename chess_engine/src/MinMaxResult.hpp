#pragma once

#include "Move.hpp"

namespace matt
{
	// TODO: Für random eine Liste benutzen mit best moves? 
	/// Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	struct MinMaxResult
	{
		Move best;
		float value;
	};
}