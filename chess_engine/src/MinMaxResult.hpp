#pragma once

#include "Move.hpp"

namespace matt
{
	// TODO: F�r random eine Liste benutzen mit best moves? 
	/// Result-Objekt f�r die Zugfindung mit dem Min-Max-Algorithmus
	struct MinMaxResult
	{
		Move best;
		float value;
	};
}