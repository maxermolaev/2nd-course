#pragma once

#include <exception>

namespace Exceptions{
    enum StatusCode{
        UNEXPECTED_ERROR_CODE = -1,
        SUCCESS = 0,
        INCORRECT_ARGS_ERROR_CODE = 1,
        INCORRECT_WAV_ERROR_CODE = 2,
        INCORRECT_CONFIG_ERROR_CODE = 3,
        UNKNOWN_COMMAND_ERROR_CODE = 4,
        INCORRECT_COMMANDS_PARAMS_ERROR_CODE = 5,
        IO_ERROR_CODE = 6,
    };

    class AbstractException : public std::exception{
    public:
        const char* what() const noexcept override = 0;
        virtual int code() const noexcept = 0;
    };

    class IncorrectWavError : public AbstractException{
    public:
        const char* what() const noexcept override {return "Incorrect .wav files";}
        int code() const noexcept override {return INCORRECT_WAV_ERROR_CODE;}
    };

    class IncorrectConfigError : public AbstractException{
    public:
        const char* what() const noexcept override {return "Incorrect config file (either path is incorrect or file couldn't be opened)";}
        int code() const noexcept override {return INCORRECT_CONFIG_ERROR_CODE;}
    };

    class UnknownCommandError : public AbstractException{
    public:
        const char* what() const noexcept override {return "Unknown command";}
        int code() const noexcept override {return UNKNOWN_COMMAND_ERROR_CODE;}
    };

    class IncorrectCommandParams :public AbstractException{
    public:
        const char* what() const noexcept override {return "Incorrect params for command(s)";}
        int code() const noexcept override {return INCORRECT_COMMANDS_PARAMS_ERROR_CODE;}
    };

    class IOError : public AbstractException{
    public:
        const char* what() const noexcept override {return "IO error";}
        int code() const noexcept override {return IO_ERROR_CODE;}
    };
}
