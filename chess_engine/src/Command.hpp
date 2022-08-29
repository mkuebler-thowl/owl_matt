#pragma once
#include "defines.hpp"
#include <string>
#include <vector>

namespace owl
{
    /**
     * Befehlsklasse zur Unterscheidung der UCI-Befehle.
     */
    class Command
    {
    public:
        /** Mögliche Befehle der UCI */
        enum Type
        {
            UCI,
            UCI_DEBUG,
            ISREADY,
            SETOPTION,
            POSITION,
            QUIT,
            GO,
            STOP,
            PONDERHIT,
            REGISTER,
            EVALUATE,
            GETVALIDMOVES,
            UCINEWGAME,
            INVALID
        };

    public:
        /**
         * Erstelle ein UCI-Befehl des Typs mit Argumenten
         * 
         * \param type Befehlstyp
         * \param args Argumente
         */
        explicit Command(const Command::Type type, const std::vector<std::string>& args);
        explicit Command() = default;
        virtual ~Command() = default;
        /**
         * Lese den Befehlstyp aus
         * 
         * \return Befehlstyp
         */
        Command::Type getType() const;
        /**
         * Setzte den Befehlstyp.
         * 
         * \param type Neuen Befehlstyp
         */
        VOID setType(Command::Type type);
        /**
         * Gebe die Liste der Argumente zurück.
         * 
         * \return Argumentliste
         */
        const std::vector<std::string>& getArgs() const;
        /**
         * Setze die Liste der Argumente.
         * 
         * \param args Neue Liste der Argumente
         */
        VOID setArgs(const std::vector<std::string>& args);
    private:
        Command::Type m_type = Command::Type::INVALID; // Befehlstyp
        std::vector<std::string> m_args; // Argumentliste
    };

    /**
     * Alle möglichen Werte, die von dem Befehl "go" zu verarbeiten sind.
     * Ist ein Integer-Wert auf 0 gesetzt, wurde dieses vom Befehl "go" nicht definiert und wird entsprechend nicht verwendet.
     * Wenn die Liste searchmoves leer ist, sollte diese auch nicht benutzt werden.
     */
    struct GoSubcommandData {
        std::vector<std::string> searchmoves; // Suche auf bestimmte Züge einschränken
        BOOL ponder = false; // Ponder-Modus
        INT32 wtime = 0; // Die Zeit in ms die Weiß noch zur Verfügung hat
        INT32 btime = 0; // Die Zeit in ms die Schwarz noch zur Verfügung hat
        INT32 winc = 0;
        INT32 binc = 0;
        INT32 movestogo = 0;
        INT32 depth = 0; // Einschränkung der Suchtiefe
        INT32 nodes = 0; // Wie viele Knoten maximal gesucht werden sollen
        INT32 mate = 0; // Suche nach einem Matt in "mate" Zügen
        INT32 movetime = 0; // Die genaue Zeit, die gesucht werden soll
        BOOL infinite = false; // Suche bis zum Stop-Befehl
    };
}