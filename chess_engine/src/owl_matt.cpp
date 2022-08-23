#pragma once

#include "defines.hpp"
#include "UniversalChessInterface.hpp"

auto main() -> owl::INT32
{
	// UCI starten:
	 owl::UniversalChessInterface uci;
	 uci.start();
}
