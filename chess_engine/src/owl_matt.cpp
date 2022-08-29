#pragma once

#include "defines.hpp"

#include "UniversalChessInterface.hpp"

auto main() -> owl::INT32
{
	owl::UniversalChessInterface uci;
	uci.start();
}
