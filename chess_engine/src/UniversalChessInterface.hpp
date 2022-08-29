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
    /**
     * Schach-Engine-Schnittstelle als Universal Chess Interface (UCI).
     */
	class UniversalChessInterface : public IChessInterface
	{
    public:
        explicit UniversalChessInterface();
        /**
         * Löse einen Befehlsstring in einen UCI-Befehl auf.
         *
         * \param commandStr Befehlsstring
         */
        virtual VOID resolveCommand(const std::string& command_str);
    private:
        /**
         * Den UCI-Befehl: "uci" verarbeiten.
         */
        VOID handleUciCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "debug" verarbeiten.
         */
        VOID handleDebugCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "isready" verarbeiten.
         */
        VOID handleIsReadyCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "setoption" verarbeiten.
         */
        VOID handleSetOptionCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "position" verarbeiten.
         */
        VOID handlePositionCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "quit" verarbeiten.
         */
        VOID handleQuitCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "go" verarbeiten.
         */
        VOID handleGoCommand(Command& cmd);
        /**
         * Die UCI-Unterbefehl von UCI-Befehl: "go" verarbeiten.
         */
        VOID handleGoSubcommand(const std::string& arg, GoSubcommandData& data);
        /**
         * Den UCI-Befehl: "stop" verarbeiten.
         */
        VOID handleStopCommand(Command& cmd);
        /**
         * Den UCI-Befehl: "ponderhit" verarbeiten.
         */
        VOID handlePonderhitCommand(Command& cmd);
        /**
         * Den UCI-Unterbefehl: "register" verarbeiten.
         */
        VOID handleRegisterCommand(Command& cmd);
        /**
         * Den UCI-Unterbefehl: "ucinewgame" verarbeiten.
         */
        VOID handleUciNewGameCommand(Command& cmd);
        /**
         * Den UCI-Unterbefehl: "eval" verarbeiten.
         */
        VOID handleEvaluateCommand(Command& cmd);
        /**
         * Den Befehl für legale Züge verarbeiten.
         * Funktionalität für das Frontend der SchwachMatt-Engine
         */
        VOID handleGetValidMovesCommand(Command& cmd);

        /**
         * Suchfunktion der UCI.
         * \param p_data Unterbefehlsdaten des Befehls "go"
         * \return 
         */
        VOID go(std::shared_ptr<GoSubcommandData> p_data);

    private:
        BOOL m_readyCheckQueued = false; // Flag zur Überprüfung ob die Engine "ready" ist

        std::thread m_minmaxThread; // Thread für die Zugfindung
	};
}