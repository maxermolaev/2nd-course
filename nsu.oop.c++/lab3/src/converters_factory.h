#pragma once

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "converters/all_converters.h"

namespace Factory{
    using CommandsDescription = std::map<std::string_view, std::pair<std::string_view, std::vector<std::string_view>>>;

    class ConvertersFactory{
    public:
        ConvertersFactory();

        const Converters::Converter& create_converter(std::string_view command_name) const; 

        const CommandsDescription& get_commands_description() const; //возвращает описание команд

        ~ConvertersFactory();
    private:
        struct Impl;
        Impl* _pimpl;
    };
}
