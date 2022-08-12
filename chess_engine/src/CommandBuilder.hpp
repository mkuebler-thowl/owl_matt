#pragma once

#include <string>
#include "defines.hpp"
#include "Command.hpp"

namespace owl
{
	class CommandBuilder
	{
	public: 
		static Command build(const std::string& commandStr);
	private:
		explicit CommandBuilder() = default;
		virtual ~CommandBuilder() = default;
	};
}