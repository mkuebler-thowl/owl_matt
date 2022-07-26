#include "UniversalChessInterface.hpp"
#include "ChessUtility.hpp"
#include "ChessEvaluation.hpp"
#include "ChessValidation.hpp"

#include <regex>

namespace owl
{
    /**
         * @brief Construct a new UniversalChessInterface::UniversalChessInterface object
         *
         */
    UniversalChessInterface::UniversalChessInterface()
        : IChessInterface()
    {
    }

    /**
     * @brief resolve a given command in string format
     *
     * @param command_str uci command
     */
    VOID UniversalChessInterface::resolveCommand(const std::string& command_str)
    {
        Command command = CommandBuilder::build(command_str);

        switch (command.getType())
        {
        case Command::UCI:
            handleUciCommand(command);
            break;
        case Command::UCI_DEBUG:
            handleDebugCommand(command);
            break;
        case Command::ISREADY:
            handleIsReadyCommand(command);
            break;
        case Command::SETOPTION:
            handleSetOptionCommand(command);
            break;
        case Command::POSITION:
            handlePositionCommand(command);
            break;
        case Command::QUIT:
            handleQuitCommand(command);
            break;
        case Command::GO:
            handleGoCommand(command);
            break;
        case Command::STOP:
            handleStopCommand(command);
            break;
        case Command::PONDERHIT:
            handlePonderhitCommand(command);
            break;
        case Command::REGISTER:
            handleRegisterCommand(command);
            break;
        case Command::UCINEWGAME:
            handleUciNewGameCommand(command);
            break;
        case Command::EVALUATE:
            handleEvaluateCommand(command);
            break;
        case Command::GETVALIDMOVES:
            handleGetValidMovesCommand(command);
            break;
        case Command::INVALID:
        default:
            std::cout << "invalid command!" << std::endl;
            break;
        }
    }

    /**
     * @brief handle the "uci" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleUciCommand(Command& cmd)
    {
        std::cout << "id name " << ENGINE_ID << std::endl;
        std::cout << "id author " << ENGINE_AUTHOR << std::endl;

        // send the settings which the engine supports via the options command
        const auto& options = m_pEngine->getOptions().getEngineOptions();
        for (const auto& it : options)
        {
            if (!it.second.isSupported)
                continue;

            std::cout << "option name " << it.first << " type " << it.second.type << " default " << it.second.default_value << std::endl;
        }

        std::cout << "uciok" << std::endl;
    }

    /**
     * @brief handle the "debug" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleDebugCommand(Command& cmd)
    {
        if (cmd.getArgs().front() == "on")
        {
            m_pEngine->setDebugMode(true);
        }
        else if (cmd.getArgs().front() == "off")
        {
            m_pEngine->setDebugMode(false);
        }
        else
        {
            throw "UCI Exception: debug can either be on or off";
        }
    }

    /**
     * @brief handle the "isready" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleIsReadyCommand(Command& cmd)
    {
        if (m_pEngine->isReady())
        {
            std::cout << "readyok" << std::endl;
        }
        else {
            m_mutex.lock();
            m_readyCheckQueued = true;
            m_mutex.unlock();
        }
    }

    /**
     * @brief handle the "setoption" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleSetOptionCommand(Command& cmd)
    {
        const std::string& name = cmd.getArgs()[0];
        const std::string& value = cmd.getArgs()[1];

        m_pEngine->setOption(name, value);
    }

    /**
     * @brief handle the "position" uci-command
     *1
     * @param cmd
     */
    VOID UniversalChessInterface::handlePositionCommand(Command& cmd)
    {
        const std::string& pos = cmd.getArgs()[0];
        BOOL fromStart = (pos == "startpos");

        std::string moves_str = cmd.getArgs()[1];
        std::vector<std::string> moves;
        size_t idx_move;
        while ((idx_move = moves_str.find(" ")) != std::string::npos)
        {
            moves.push_back(moves_str.substr(0, idx_move));
            moves_str.erase(0, idx_move + 1);
        }
        moves.push_back(moves_str);

        // set the position of the board to "pos"
        Position& position = m_pEngine->getPosition();
        if (fromStart)
        {
            position = ChessUtility::fenToPosition(STARTPOS_FEN);
        }
        else
        {
            position = ChessUtility::fenToPosition(pos);
        }

        for (auto& move_str : moves)
        {
            if (move_str == "0000") continue;
            Move move = ChessUtility::stringToMove(move_str, &position);
            //Move move = ChessUtility::stringToMove(move_str);
            m_pEngine->getPosition().applyMove(move);
        }
    }

    /**
     * @brief handle the "quit" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleQuitCommand(Command& cmd)
    {
        m_mutex.lock();
        m_isRunning = false;
        m_mutex.unlock();

        this->handleStopCommand(cmd);

        std::cout << "quit ok" << std::endl;
    }

    /**
     * @brief handle the "go" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleGoCommand(Command& cmd)
    {
        std::shared_ptr<GoSubcommandData> subcommandData = std::make_shared<GoSubcommandData>();
        const std::vector<std::string>& args = cmd.getArgs();
        for (const std::string& arg : args)
        {
            handleGoSubcommand(arg, *subcommandData);
        }

        if (m_pEngine->isReady()) {
            if (m_minmaxThread.joinable()) {
                m_minmaxThread.join();
            }
        }

        //go(subcommandData);
        this->m_minmaxThread = std::thread(&UniversalChessInterface::go, this, subcommandData);
    }

    /**
     * @brief handle the subcommands that follow the "go" uci-command
     *
     * @param subcommand
     */
    VOID UniversalChessInterface::handleGoSubcommand(const std::string& arg, GoSubcommandData& data)
    {
        // first the boolean flags
        if (arg == "ponder")
        {
            data.ponder = true;
            return;
        }

        if (arg == "infinite")
        {
            data.infinite = true;
            return;
        }

        std::smatch match;

        // check if it is the searchmoves command since it is the only command left that doesnt have an integer value
        if (arg.find("searchmoves") != std::string::npos)
        {
            if (std::regex_search(arg, match, std::regex("([a-hA-H0-8x\\+]{4,6})")))
            {
                for (size_t i = 1; i < match.size(); i++)
                {
                    if (match.str(i).empty())
                        continue;

                    data.searchmoves.push_back(match.str(i));
                }
            }
            return;
        }

        // all subcommands that are left have an integer value
        // get the value using regex
        int value;
        if (std::regex_search(arg, match, std::regex("-?\\d+")))
        {
            value = atoi(match.str(0).data());
        }
        else
        {
            return;
        }

        if (arg.find("wtime") != std::string::npos)
        {
            data.wtime = value;
            return;
        }

        if (arg.find("btime") != std::string::npos)
        {
            data.btime = value;
            return;
        }

        if (arg.find("winc") != std::string::npos)
        {
            data.winc = value;
            return;
        }

        if (arg.find("binc") != std::string::npos)
        {
            data.binc = value;
            return;
        }

        if (arg.find("movestogo") != std::string::npos)
        {
            data.movestogo = value;
            return;
        }

        if (arg.find("depth") != std::string::npos)
        {
            data.depth = value;
            return;
        }

        if (arg.find("nodes") != std::string::npos)
        {
            data.nodes = value;
            return;
        }

        if (arg.find("mate") != std::string::npos)
        {
            data.mate = value;
            return;
        }

        if (arg.find("movetime") != std::string::npos)
        {
            data.movetime = value;
            return;
        }
    }

    /**
     * @brief handle the "stop" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleStopCommand(Command& cmd)
    {
        m_pEngine->stop();

        if (m_minmaxThread.joinable()) {
            m_minmaxThread.join();
        }

        //std::cout << "command not implemented yet" << std::endl;
    }

    /**
     * @brief handle the "ponderhit" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handlePonderhitCommand(Command& cmd)
    {
        std::cout << "command not implemented yet" << std::endl;
    }

    /**
     * @brief handle the "register" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleRegisterCommand(Command& cmd)
    {
        std::cout << "command not implemented yet" << std::endl;
    }

    /**
     * @brief handle the "ucinewgame" uci-command
     *
     * @param cmd
     */
    VOID UniversalChessInterface::handleUciNewGameCommand(Command& cmd)
    {
        m_pEngine->getPosition() = ChessUtility::fenToPosition(STARTPOS_FEN);
    }

    VOID UniversalChessInterface::handleEvaluateCommand(Command& cmd)
    {
        std::cout << ChessEvaluation::evaluate(m_pEngine->getPosition(), EVAL_FT_STANDARD) << std::endl;
    }

    VOID UniversalChessInterface::handleGetValidMovesCommand(Command& cmd)
    {
        MOVE_LIST moves = ChessValidation::getValidMoves(m_pEngine->getPosition(), m_pEngine->getPosition().getPlayer());
        for (auto& move : moves)
        {
            std::cout << ChessUtility::moveToString(move) << ", ";
        }

        if (!moves.empty()) std::cout << ChessUtility::moveToString(moves.back()) << std::endl;
    }

    VOID UniversalChessInterface::go(std::shared_ptr<GoSubcommandData> p_data)
    {
        INT32 depth = p_data->depth == 0 ? MAX_DEPTH : p_data->depth;

        Position pos = m_pEngine->getPosition();

        auto result = m_pEngine->searchMove(m_pEngine->getPosition().getPlayer(), depth, FT_STANDARD);

        auto best = result.first;
        auto value = result.second;

        std::cout << "info depth 0 " << ChessEvaluation::evaluate(pos, m_pEngine->getPlayer(), EVAL_FT_STANDARD, false) << "\n";
        std::cout << "info score " << value << " cp " << ChessUtility::convertToCentipawns(value) << std::endl;
        std::cout << "bestmove " << ChessUtility::moveToString(best) << std::endl;

        if (m_readyCheckQueued) {
            std::cout << "readyok" << std::endl;
            m_mutex.lock();
            m_readyCheckQueued = false;
            m_mutex.unlock();
        }
    }
}
