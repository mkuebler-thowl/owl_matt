#pragma once

#include <string>
#include <memory>
#include <mutex>
#include "defines.hpp"

#include "ChessEngine.hpp"

namespace owl
{
	/**
	 * Schnittstelle für ein Schach-Engine-Interface
	 */
	class IChessInterface
	{
	public:
		explicit IChessInterface();
		virtual ~IChessInterface() = default;
		/**
		 * Starte das Schach-Engine-Interface.
		 */
		VOID start();
		/**
		 * Löse einen Befehlsstring auf.
		 * 
		 * \param commandStr Befehlsstring
		 */
		virtual VOID resolveCommand(const std::string& commandStr) = 0;
	protected:
		BOOL m_isRunning = false; // Läuft die Engine
		std::shared_ptr<ChessEngine> m_pEngine; // Das Engine-Objekt
		std::mutex m_mutex; // Mutex zur Thread-Blockierung
	};
}