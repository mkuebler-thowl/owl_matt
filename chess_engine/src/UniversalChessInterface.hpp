#pragma once

#include "defines.hpp"

#include <memory>
#include <thread>

#include "ChessEngine.hpp"
#include "IChessInterface.hpp"
#include "Command.hpp"
#include "CommandBuilder.hpp"

namespace owl
{
	class UniversalChessInterface : public IChessInterface
	{
    public:
        explicit UniversalChessInterface();

        virtual VOID resolveCommand(const std::string& command_str);

    private:
        VOID handleUciCommand(Command& cmd);
        VOID handleDebugCommand(Command& cmd);
        VOID handleIsReadyCommand(Command& cmd);
        VOID handleSetOptionCommand(Command& cmd);
        VOID handlePositionCommand(Command& cmd);
        VOID handleQuitCommand(Command& cmd);
        VOID handleGoCommand(Command& cmd);
        VOID handleGoSubcommand(const std::string& arg, GoSubcommandData& data);
        VOID handleStopCommand(Command& cmd);
        VOID handlePonderhitCommand(Command& cmd);
        VOID handleRegisterCommand(Command& cmd);
        VOID handleUciNewGameCommand(Command& cmd);
        VOID handleEvaluateCommand(Command& cmd);
        VOID handleGetValidMovesCommand(Command& cmd);

        VOID go(std::shared_ptr<GoSubcommandData> p_data);

    private:
        BOOL m_isReady = false;
        BOOL m_isInitialized = false;
        BOOL m_debugMode = false;
        BOOL m_readyCheckQueued = false;

        std::thread m_minmaxThread;
	};
}