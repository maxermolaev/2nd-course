#include "streams.h"

Streams::InputStream::InputStream(std::string_view file_path): _reader(WAVManagement::WAVReader(file_path)) {} // конструктор

WAVFormatInfo::Sample Streams::InputStream::read_element(){ // читаем элемент
    return _reader.read_sample();
}

void Streams::InputStream::skip(size_t count){ // пропускаем элементы
    _reader.skip(count);
}

void Streams::InputStream::skip(size_t start_sample, size_t end_sample){ // пропускаем промежуток элементов
    _reader.skip(start_sample, end_sample);
}

float Streams::InputStream::get_duration_s() const{ // получаем длительность
    return _reader.get_duration_s();
}

size_t Streams::InputStream::get_size() const{ // получаем размер
    return _reader.get_samples_count();
}

bool Streams::InputStream::available() const{ // есть ли ещё элементы
    return _reader.available();
}

Streams::OutputStream::OutputStream(std::string_view file_path): _writer(WAVManagement::WAVWriter(file_path)) {} // конструктор

void Streams::OutputStream::write(WAVFormatInfo::Sample sample){ // записываем элемент
    _writer.write_sample(sample);
}
