#pragma once //для чтения и записи wav

#include <string>
#include "wav.h"

namespace Streams{ 
    class InputStream{  //класс для чтения
    public:
        explicit InputStream(std::string_view file_path);

        WAVFormatInfo::Sample read_element();

        void skip(size_t count);

        void skip(size_t start_sample, size_t end_sample);

        float get_duration_s() const;

        size_t get_size() const;

        bool available() const;
    private:
        WAVManagement::WAVReader _reader;
    };

    class OutputStream{ //класс для записи
    public:
        explicit OutputStream(std::string_view file_path);

        void write(WAVFormatInfo::Sample sample);
    private:
        WAVManagement::WAVWriter _writer;
    };
}
