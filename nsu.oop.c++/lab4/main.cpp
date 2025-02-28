#include <fstream>
#include <iostream>
#include <string>
#include <tuple> // для работы с кортежами
#include <sstream>

template <std::size_t Index = 0, typename Ch, typename Tr, typename... Args> //создаем рекурсивную функцию
typename std::enable_if<Index == sizeof...(Args), std::ostream&>::type //если индекс равен количеству аргументов
printTupleRecursive(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) { //выводим кортеж
    return os;
}

template <std::size_t Index = 0, typename Ch, typename Tr, typename... Args>
typename std::enable_if<Index < sizeof...(Args), std::ostream&>::type
printTupleRecursive(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) {
    os << std::get<Index>(t) << ' ';
    return printTupleRecursive<Index + 1>(os, t);
}

template <typename Ch, typename Tr, typename... Args>
std::ostream& operator<<(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t) { //оператор вывода
    return printTupleRecursive(os, t);
}

template <typename... Args>

class CSVParser { 
public:
    using value_type = std::tuple<Args...>; //кортеж
    using reference = const value_type&; //ссылка на константный кортеж
    explicit CSVParser(std::ifstream& ifs, size_t line_offset = 0, char column_delim = ' '): file_(ifs), line_offset_(line_offset), column_delim_(column_delim) {
        if (!file_.is_open()) throw std::ifstream::failure("File is not open");
    }

    class Iterator { //итератор для ленивого чтения
    public:
        Iterator() : file_(nullptr){};
        Iterator(std::istream& parent, int offset, char column_delim) : file_(&parent), line_number_(offset), column_delim_(column_delim) {
            for (size_t i = 0; i < offset; ++i) {
                try {
                    if (!getline(*file_, str_)) { //конец файла
                        file_ = nullptr;
                        throw std::invalid_argument("End of file");
                    }
                } catch (std::invalid_argument) {
                    std::cout << "End of file after offset" << std::endl;
                    exit(0);
                }
            }
            ++*this;
        };

        bool operator!=(const Iterator& y) { return !(*this == y); } //проверяем не равны ли 2 итератора
        bool operator==(const Iterator& y) { return (*this).file_ == y.file_; } 
        reference operator*() const { return output_; } //возвращаем текущий элемент, на который указывает итератор

        const Iterator operator++() { //переход к следующей строке
            if (!getline(*file_, str_)) file_ = nullptr;

            ++line_number_;
            std::istringstream line(str_);
            std::string element;
            size_t index = 0;
            try {
                for_each(output_, [&](auto&& x) { //разбиваем строку на части и преобразовываем в соответствующие типы
                    if (getline(line, element, column_delim_)) {
                        auto buf = x;
                        decltype(buf) tmpValue;
                        std::istringstream tmpStream(element);
                        tmpStream >> tmpValue;
                        x = tmpValue;
                        std::ostringstream checkType;
                        checkType << tmpValue;
                        ++index;
                        if (checkType.str() != element) {
                            throw index;
                        }
                    }
                });
            } catch (size_t idx) {
                std::cout << "Invalid argument in " << idx << " column of "
                          << line_number_ << " line " << std::endl;
            }
            return *this;
        }
        template <typename TupleT,typename Fn> //тип кортежа и тип функции
        void for_each(TupleT&& tp, Fn&& fn) {
            std::apply(
                    [&fn]<typename... T>(T&&... args) {
                        (fn(std::forward<T>(args)), ...); //применяем функцию ко всем элементам 
                    },
                    std::forward<TupleT>(tp));  
        }

    private:
        size_t line_number_;
        char column_delim_;
        std::string str_;
        value_type output_; //текущая строка в виде кортежа
        std::istream* file_;
    };
    Iterator begin() { return Iterator(file_, line_offset_, column_delim_); } //начало данных
    Iterator end() { return Iterator(); } 

private:
    std::ifstream& file_;
    size_t line_offset_;
    char column_delim_;
};

int main(int argc, const char* argv[]) {
    std::ifstream file("test1.csv");
    int offset = 0;
    char delim = ',';
    CSVParser<int, std::string> parser(file, offset, delim);
    for (auto it = parser.begin(); it != parser.end(); ++it) {
        std::cout << *it << std::endl;
    }
    file.close();
    return 0;
}

