#pragma once

#include <string>
#include "defines.hpp"
#include "Command.hpp"

namespace owl
{
	/**
	 * Hilfsklasse zum Parsen von UCI-Befehlen.
	 */
	class CommandBuilder
	{
	public: 
		/**
		 * Erstellt ein Befehl aus einem Befehlsstring.
		 * 
		 * \param command_str String im UCI-Format
		 * \return Command Befehlsobjekt
		 */
		static Command build(const std::string& commandStr);
	private:
		explicit CommandBuilder() = default;
		virtual ~CommandBuilder() = default;
	};
}