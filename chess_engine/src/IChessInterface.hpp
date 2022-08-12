#pragma once

#include <string>
#include <memory>
#include <mutex>
#include "defines.hpp"

#include "ChessEngine.hpp"

namespace owl
{
	class IChessInterface
	{
	public:
		explicit IChessInterface();
		virtual ~IChessInterface() = default;

		VOID start();

		virtual VOID resolveCommand(const std::string& commandStr) = 0;
	protected:
		BOOL m_isRunning = false;
		std::shared_ptr<ChessEngine> m_pEngine;
		std::mutex m_mutex;
	};
}