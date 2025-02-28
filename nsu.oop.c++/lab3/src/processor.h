#pragma once //обработка команд

#include <string>
#include <vector>
#include <cmath>
#include "converters_factory.h"

namespace Processing{
    const std::string_view LEFT_ARROW = "<-";  //начало времени
    const std::string_view RIGHT_ARROW = "->"; //конец времени

    const float LEFT_TIME_ARROW_ALIAS = NAN; 
    const float RIGHT_TIME_ARROW_ALIAS = INFINITY; 

    class Processor{ //класс для обработки команд
    public:
        Processor(const std::string_view& config,
                  const std::string_view& out,
                  const std::string_view& in,
                  const std::vector<std::string>& additional_files, //дополнительные файлы
                  const Factory::ConvertersFactory& factory); //фабрика конвертеров

        void process() const; //обработка

        ~Processor();
    private:
        struct Impl;
        Impl* _pimpl;   
    };
}
