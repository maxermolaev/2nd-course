#include <fstream>
#include "custom_exceptions.h"
#include "utils.h"
#include "wav.h"

constexpr size_t WAV_READER_BUFFER_SIZE = 1024;

constexpr size_t WAV_WRITER_BUFFER_SIZE = 1024;

constexpr size_t MIN_SIZE_OF_HEADERS = 44;

struct RIFFChunk{ 
    uint32_t chunk_size; 
    uint32_t format; 
};

struct FMTSubchunk{ //описываем формат данных
    uint32_t subchunk_size; // Размер оставшейся части подзаголовка
    uint16_t audio_format; // Формат аудио: 1 для PCM
    uint16_t num_channels; // 1 канал (моно)
    uint32_t sample_rate; // Частота дискретизации: поддерживается только 44100 Гц
    uint32_t byte_rate; // Количество байт, передаваемых в секунду
    uint16_t block_align; // Размер одного блока данных
    uint16_t bits_per_sample; // Глубина звука - поддерживается 16 бит
};

void validate_riff_chunk(const RIFFChunk& riff_chunk, size_t file_size){ //проверяем корректность заголовка
    const bool check1 = static_cast<size_t>(riff_chunk.chunk_size) == file_size - 8;
    const bool check2 = riff_chunk.format == WAVFormatInfo::SUPPORTED_RIFF_FORMAT;

    const bool is_valid = check1 && check2;

    if (!is_valid){
        throw Exceptions::IncorrectWavError();
    }
}
void read_riff_chunk(std::ifstream& wav_file){
    RIFFChunk riff_chunk{};

    wav_file.read(reinterpret_cast<char*>(&riff_chunk), sizeof(riff_chunk));

    size_t file_size = Utils::get_file_size(wav_file);

    validate_riff_chunk(riff_chunk, file_size);
}

void validate_fmt_subchunk(const FMTSubchunk& fmt_subchunk){ //что подчанк fmt (форматный подчанк) WAV-файла соответствует заранее заданным значениям
    const bool check1 = fmt_subchunk.subchunk_size == WAVFormatInfo::SUPPORTED_FMT_SUBCHUNK_SIZE;
    const bool check2 = fmt_subchunk.audio_format == WAVFormatInfo::SUPPORTED_FMT_AUDIO_FORMAT;
    const bool check3 = fmt_subchunk.num_channels == WAVFormatInfo::SUPPORTED_NUMBER_OF_CHANNELS;
    const bool check4 = fmt_subchunk.sample_rate == WAVFormatInfo::SUPPORTED_SAMPLE_RATE;
    const bool check5 = fmt_subchunk.byte_rate == WAVFormatInfo::SUPPORTED_BYTE_RATE;
    const bool check6 = fmt_subchunk.block_align == WAVFormatInfo::SUPPORTED_BLOCK_ALIGN;
    const bool check7 = fmt_subchunk.bits_per_sample == WAVFormatInfo::SUPPORTED_BITS_PER_SAMPLE;

    const bool is_valid = check1 && check2 && check3 && check4 && check5 && check6 && check7;

    if (!is_valid){
        throw Exceptions::IncorrectWavError();
    }
}
void read_fmt_subchunk(std::ifstream& wav_file){ //читаем форматный подчанк
    FMTSubchunk fmt_subchunk{};

    wav_file.read(reinterpret_cast<char*>(&fmt_subchunk), sizeof(fmt_subchunk));

    validate_fmt_subchunk(fmt_subchunk);
}

void read_extra_data_subchunk(std::ifstream& wav_file){ //считываем дополнительные данные в буфер
    uint32_t subchunk_size;

    wav_file.read(reinterpret_cast<char*>(&subchunk_size), sizeof(subchunk_size));

    auto buffer = new char[subchunk_size];

    wav_file.read(buffer, subchunk_size);

    delete[] buffer;
}
void read_data_subchunk(std::ifstream& wav_file, std::fstream::pos_type& data_start_position, size_t& samples_count){ //считываем аудио сэмлы
    uint32_t data_subchunk_samples_count;

    wav_file.read(reinterpret_cast<char*>(&data_subchunk_samples_count), sizeof(data_subchunk_samples_count));

    data_subchunk_samples_count /= 2; //сэмпл = 2 байта

    data_start_position = wav_file.tellg(); //текущуя позиция чтения в файле
    samples_count = data_subchunk_samples_count;

    for (unsigned int i = 0; i < data_subchunk_samples_count; i++){
        uint16_t sample;
        wav_file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
    }
}

WAVManagement::WAVInfo WAVManagement::parse_wav(std::string_view file_path){ //получаем данные(кол-во сэмлов,длительность аудио)
    std::fstream::pos_type data_start_position; 
    size_t samples_count;

    std::ifstream wav_file(file_path.data(), std::ios::binary); //открываем файл в бинарном виде

    if (!wav_file.is_open() || Utils::is_file_empty(wav_file)){
        throw Exceptions::IncorrectWavError();
    }

    bool met_riff = false;
    bool met_fmt = false;
    bool met_data = false;
    bool met_extra_data = false;

    uint32_t current_id;
    while (wav_file.read(reinterpret_cast<char*>(&current_id), WAVFormatInfo::ID_SIZE) && wav_file){ //Проверяем, что субчанки идут в правильном порядке
        const bool incorrect_order_of_chunks = current_id != WAVFormatInfo::RIFF_ID && !met_riff;
        if (incorrect_order_of_chunks){
            throw Exceptions::IncorrectWavError();
        }

        const bool separated_chunks = (current_id == WAVFormatInfo::RIFF_ID && met_riff) || //Проверяем, не встретились ли повторно уже прочитанные субчанки
                                      (current_id == WAVFormatInfo::FMT_ID && met_fmt) ||
                                      (current_id == WAVFormatInfo::DATA_ID && met_data) ||
                                      (current_id == WAVFormatInfo::LIST_ID && met_extra_data);

        if (separated_chunks){
            throw Exceptions::IncorrectWavError();
        }

        switch (current_id){ //проверяем считанные данные
            case WAVFormatInfo::RIFF_ID:
                met_riff = true;
                read_riff_chunk(wav_file);
                break;
            case WAVFormatInfo::FMT_ID:
                met_fmt = true;
                read_fmt_subchunk(wav_file);
                break;
            case WAVFormatInfo::LIST_ID:
                met_extra_data = true;
                read_extra_data_subchunk(wav_file);
                break;
            case WAVFormatInfo::DATA_ID:
                met_data = true;
                read_data_subchunk(wav_file, data_start_position, samples_count);
                break;
            default:
                throw Exceptions::IncorrectWavError();
        }
    }

    if (!wav_file.eof()){ // если не конец файла
        throw Exceptions::IncorrectWavError();
    }

    float duration_s = static_cast<float>(samples_count) / static_cast<float>(WAVFormatInfo::SUPPORTED_SAMPLE_RATE); //длительность

    return WAVInfo{
        duration_s,
        samples_count,
        data_start_position
    };
}

struct WAVManagement::WAVReader::Impl{
    WAVFormatInfo::Sample buffer[WAV_READER_BUFFER_SIZE]; // массив, в который загружаются сэмплы аудиоданных

    size_t index_in_buffer; //текущая позиция
    size_t read_samples_count; //кол-во прочитанных
    float duration_s;
    size_t total_samples_count;
    std::ifstream ifstream; //поток чтения
    void read();
    void move();
    bool available() const;
    WAVFormatInfo::Sample get_sample();
    void skip(size_t count);
};

void WAVManagement::WAVReader::Impl::move(){ //перемещение по буферу и за загрузка новых данных, когда буфер заполняется
    index_in_buffer++; 

    if (index_in_buffer == WAV_READER_BUFFER_SIZE){
        read();
        index_in_buffer = 0;
        read_samples_count = static_cast<size_t>(ifstream.gcount());
    }
}

bool WAVManagement::WAVReader::Impl::available() const{ //проверка, есть ли еще данные в текущем буфере, которые можно обработать
    return read_samples_count > index_in_buffer;
}

WAVFormatInfo::Sample WAVManagement::WAVReader::Impl::get_sample(){ //извлечение одного сэмпла из буфера
    WAVFormatInfo::Sample result = buffer[index_in_buffer];

    move();

    return result;
}

void WAVManagement::WAVReader::Impl::skip(size_t count){ //пропускаем count сэмлов
    for (unsigned int i = 0; i < count; i++){
        move();
    }
}

void WAVManagement::WAVReader::Impl::read(){ //чтение данных из потока в память
    ifstream.read(reinterpret_cast<char*>(&buffer),
                  static_cast<std::streamsize>(WAV_READER_BUFFER_SIZE * sizeof(WAVFormatInfo::Sample)));
}

WAVManagement::WAVReader::WAVReader(std::string_view file_path){
    WAVManagement::WAVInfo info = WAVManagement::parse_wav(file_path);

    _pimpl = new Impl{{},
                      0,
                      0,
                      info.duration_s,
                      info.samples_count,
                      std::ifstream(file_path.data(), std::fstream::binary)};

    _pimpl->ifstream.seekg(info.data_start_position);//перемещаемся, где начинаются сэмплы

    _pimpl->read();

    _pimpl->read_samples_count = static_cast<size_t>(_pimpl->ifstream.gcount()); //сколько сэмплов было успешно считано в текущем буфере
}

float WAVManagement::WAVReader::get_duration_s() const{//длительность
    return _pimpl->duration_s;
}

size_t WAVManagement::WAVReader::get_samples_count() const{ //кол-во сэмплов 
    return _pimpl->total_samples_count;
}

WAVFormatInfo::Sample WAVManagement::WAVReader::read_sample(){ //читаем сэмпл
    return _pimpl->get_sample();
}

void WAVManagement::WAVReader::skip(size_t count){
    _pimpl->skip(count);
}

void WAVManagement::WAVReader::skip(size_t start_sample, size_t end_sample){//скипаем промежуток сэмплов
    _pimpl->skip(end_sample - start_sample);
}

bool WAVManagement::WAVReader::available() const{
    return _pimpl->available();
}

WAVManagement::WAVReader::~WAVReader(){
    delete _pimpl;
}

struct WAVManagement::WAVWriter::Impl{ //запись файла
    WAVFormatInfo::Sample buffer[WAV_WRITER_BUFFER_SIZE];
    size_t index_in_buffer;
    size_t elements_in_buffer_count;
    std::ofstream ofstream;
    size_t samples_written;
    void dump_buffer(); // из буфера в файл
    void write_sample(WAVFormatInfo::Sample sample);
    void write_default_wav_header(size_t samples_count);
    void replace_samples_count_in_file();
};

void WAVManagement::WAVWriter::Impl::dump_buffer(){
    ofstream.write(reinterpret_cast<char*>(&buffer),
                   static_cast<std::streamsize>(elements_in_buffer_count * sizeof(WAVFormatInfo::Sample)));

    index_in_buffer = 0;
    elements_in_buffer_count = 0;
}

void WAVManagement::WAVWriter::Impl::write_sample(WAVFormatInfo::Sample sample){ 
    buffer[index_in_buffer] = sample;

    index_in_buffer++;
    elements_in_buffer_count++;
    samples_written++;

    if (index_in_buffer == WAV_WRITER_BUFFER_SIZE){
        dump_buffer();
    }
}

template<typename T>
void write_value(std::ofstream& file, T value){ //Записывает в файл бинарное представление значения
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}

uint32_t calculate_riff_chunk_size(size_t samples_count){ // размер RIFF-чанка в WAV-файле
    return static_cast<uint32_t>(samples_count * WAVFormatInfo::SUPPORTED_BLOCK_ALIGN + MIN_SIZE_OF_HEADERS - 8);
}
uint32_t calculate_data_subchunk_size(size_t samples_count){ //размер данных в data-чанке WAV-файла
    return static_cast<uint32_t>(samples_count * 2);
}

void WAVManagement::WAVWriter::Impl::write_default_wav_header(size_t samples_count){ //записи стандартного заголовка WAV-файла
    auto file_size = calculate_riff_chunk_size(samples_count);
    write_value(ofstream, WAVFormatInfo::RIFF_ID);
    write_value(ofstream, file_size);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_RIFF_FORMAT);
    write_value(ofstream, WAVFormatInfo::FMT_ID);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_FMT_SUBCHUNK_SIZE);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_FMT_AUDIO_FORMAT);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_NUMBER_OF_CHANNELS);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_SAMPLE_RATE);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_BYTE_RATE);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_BLOCK_ALIGN);
    write_value(ofstream, WAVFormatInfo::SUPPORTED_BITS_PER_SAMPLE);
    write_value(ofstream, WAVFormatInfo::DATA_ID);
    write_value(ofstream, calculate_data_subchunk_size(samples_count));
}

void WAVManagement::WAVWriter::Impl::replace_samples_count_in_file(){ //замена числа сэмплов в заголовке WAV-файла

    ofstream.seekp(WAVFormatInfo::ID_SIZE);
    write_value(ofstream, calculate_riff_chunk_size(samples_written));

    ofstream.seekp(40);
    write_value(ofstream, calculate_data_subchunk_size(samples_written));
}

WAVManagement::WAVWriter::WAVWriter(std::string_view file_path){ //создание объекта для записи WAV-файла
    _pimpl = new Impl{{},
                      0,
                      0,
                      std::ofstream(file_path.data(), std::fstream::binary),
                      0};

    _pimpl->write_default_wav_header(0);
}

void WAVManagement::WAVWriter::write_sample(WAVFormatInfo::Sample sample){ //запись аудио
    _pimpl->write_sample(sample);
}

WAVManagement::WAVWriter::~WAVWriter(){ 
    _pimpl->dump_buffer();

    _pimpl->replace_samples_count_in_file();

    delete _pimpl;
}
