#pragma once

#include "defines.hpp"

#include <string>
#include <unordered_map>

namespace owl
{
    struct EngineOption {
        BOOL isSupported;
        std::string value;
        std::string default_value;
        std::string type;
    };

    class EngineOptions {
    public:
        //constructor/destructor
        explicit EngineOptions();
        virtual ~EngineOptions();

        BOOL isSupported(const std::string& id) const;
        VOID setValue(const std::string& id, const std::string& value);
        const std::string& getValue(const std::string& id) const;

        const EngineOption& getOption(const std::string& id) const;
        const std::unordered_map<std::string, EngineOption>& getEngineOptions() const;

    private:
        //maps option-id to option
        std::unordered_map<std::string, EngineOption> m_engineOptions;
    };
}