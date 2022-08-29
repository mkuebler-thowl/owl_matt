#pragma once

#include "defines.hpp"

#include <string>
#include <unordered_map>

namespace owl
{
    /**
     * Sturktur zur �berpr�fung ob Engine-Optionen unterst�tzt werden.
     */
    struct EngineOption {
        BOOL isSupported;
        std::string value;
        std::string default_value;
        std::string type;
    };

    /**
     * Klasse der Engine-Option.
     */
    class EngineOptions {
    public:
        //constructor/destructor
        explicit EngineOptions();
        virtual ~EngineOptions();
        /**
         * Wird folgendes Feature von der Engine unterst�tzt?
         * 
         * \param Schl�ssel 
         */
        BOOL isSupported(const std::string& id) const;
        /**
         * Ver�ndere einen Wert der jeweiligen Engine-Option
         * 
         * \param id Schl�ssel
         * \param value korrespondierender Wert, der gesetzt werden soll
         */
        VOID setValue(const std::string& id, const std::string& value);
        /**
         * Gebe den Wert einer Engine-Option des entsprechendes Schl�ssel zur�ck.
         * 
         * \param id Schl�ssel
         * \return Wert
         */
        const std::string& getValue(const std::string& id) const;
        /**
         * Gebe die Engine-Option zur�ck
         * 
         * \param id Schl�ssel
         * \return EngineOption-Objekt
         */
        const EngineOption& getOption(const std::string& id) const;
        /**
         * Gebe die Liste aller Engine-Optionen raus.
         * 
         * \return Engine-Optionen mit Schl�ssel 
         */
        const std::unordered_map<std::string, EngineOption>& getEngineOptions() const;
    private:
        std::unordered_map<std::string, EngineOption> m_engineOptions; // Engine-Optionen
    };
}