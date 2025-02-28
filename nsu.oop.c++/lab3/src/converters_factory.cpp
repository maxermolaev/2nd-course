#include <functional>
#include <vector>
#include "utils.h"
#include "custom_exceptions.h"
#include "converters_factory.h"

struct Factory::ConvertersFactory::Impl{ 
    struct CommandInfo{
        std::string_view description;
        std::vector<std::string_view> examples;
        std::function<Converters::Converter(void)> converter_creation; //функция создания конвертера для данной команды
    };

    std::map<std::string_view, CommandInfo> commands_full_info;

    CommandsDescription commands_description;

    Converters::Converter create_converter_by_command(std::string_view command_name);
};

Converters::Converter Factory::ConvertersFactory::Impl::create_converter_by_command(std::string_view command_name){  //создаем конвертер по команде
    for (const auto& [current_command, info] : commands_full_info){
        if (current_command == command_name){
            return info.converter_creation();
        }
    }

    throw Exceptions::UnknownCommandError();
}

template<typename T> 
Converters::Converter create(){ 
    return std::make_unique<T>();
}

Factory::ConvertersFactory::ConvertersFactory(){
    _pimpl = new Impl;

    _pimpl->commands_full_info["mute"] = {
            "mutes fragment of file",
            {"mute <fragment_of_file>"},
            create<RawMuteConverter>
    };

    _pimpl->commands_full_info["mix"] = {
            "mixes two files",
            {"<additional_file> <place_in_input_file> <fragment_of_additional_file>"},
            create<RawMixConverter>
    };

    _pimpl->commands_full_info["cut"] = {
            "removes fragment of file",
            {"<fragment_of_file>"},
            create<RawCutConverter>
    };

    for (const auto& [name, data] : _pimpl->commands_full_info){ //заполняем описание команд
        _pimpl->commands_description[name] = {data.description, data.examples};
    }
}

const Converters::Converter& Factory::ConvertersFactory::create_converter(std::string_view command_name) const{
    static std::map<std::string_view, Converters::Converter> converters;

    if (!Utils::contains(converters, command_name)){
        converters[command_name] = _pimpl->create_converter_by_command(command_name); //создаем конвертер
    }

    return converters.at(command_name);
}

const Factory::CommandsDescription& Factory::ConvertersFactory::get_commands_description() const{
    return _pimpl->commands_description; //возвращаем описание команд
}

Factory::ConvertersFactory::~ConvertersFactory(){
    delete _pimpl;
}
