 #include "../custom_exceptions.h"
#include "converters_utils.h"
#include "mute_converter.h"

void RawMuteConverter::convert(std::string_view command,
                                 Streams::OutputStream& output_stream,
                                 Streams::InputStream& input_stream,
                                 const Converters::ConverterParams& params) const{

    if (command == "mute" && params.size() != 2){
        throw Exceptions::IncorrectCommandParams();
    }

    size_t start_time_index = command == "mute" ? 0 : 1;
    size_t end_time_index = command == "mute" ? 1 : 2;

    float coefficient = 0;  // Параметр коэффициента для mute всегда равен 0

    // Вычисляем начальный и конечный индекс сэмплов для интервала
    auto start_sample = calculate_sample_index_by_time(
            convert_any<float>(params[start_time_index]),
            input_stream
            );

    auto end_sample = calculate_sample_index_by_time(
            convert_any<float>(params[end_time_index]),
            input_stream
    );

    // Проверяем корректность интервала времени
    check_time_fragment(start_sample, end_sample, input_stream);

    // Заполняем выходной поток "тишиной" (нулевыми значениями) в интервале между start_sample и end_sample
    for (size_t i = start_sample; i < end_sample; i++){
        WAVFormatInfo::Sample silent_sample = 0;
        output_stream.write(silent_sample);
    }
    
    while (input_stream.available()){
        output_stream.write(input_stream.read_element());
    }
}
