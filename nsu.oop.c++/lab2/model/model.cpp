#include "model.h"

int numByte(int ind) { //вычисляем кол-во байтов для хранения битов
    return ind % 8 != 0 || ind == 0 ? ind / 8 + 1 : ind / 8;
}

int numInByte(int column) { //вычисляем позицию бита внутри байта для заданного индекса столбца
    if (column < 8) {
        return column;
    }
    return column % 8 == 0 ? 0 : column % 8;
}

bool checkBit(unsigned char byte, int pos) { //проверяет, установлен ли бит в позиции в переданном байте
    return static_cast<bool>((1 << (7 - pos)) & byte);
}

void setTrue(unsigned char &byte, int pos) { //устанавливает бит в переданном байте в значение 1
    byte |= (1 << (7 - pos));
}

void setFalse(unsigned char &chank, int pos) { //в 0
    chank &= ~(1 << (7 - pos));
}

int normalize(int coord, int size) { //следим за выходом клетки за поле
    int k = coord / size;
    k -= coord < (-1) * size ? 1 : 0;
    if (coord >= 0 || coord < (-1) * size) {
        coord -= k * size;
    } else {
        coord += size;
    }
    return coord;
}

Field::Field() { //инициализируем объект класса Field, создавая (field_) и заполняя его нулями
    field_.resize(size_ * numByte(size_), 0);
}

Field::Field(std::vector<Point> coords) { //создаёт игровое поле, где "живые" клетки инициализируются из переданного вектора координат
    field_.resize(size_ * numByte(size_), 0);
    for (auto& i: coords) {
        i.x = normalize(i.x, size_);
        i.y = normalize(i.y, size_);
        int num_chank = i.x * numByte(size_) + numByte(i.y); //номер байта
        int pos = numInByte(i.y);
        num_chank += i.y % 8 == 0 && i.y != 0? 0: -1;
        if (checkBit(field_[num_chank], pos)) {
            throw std::invalid_argument("Coordinates repeate");
        } else {
            setTrue(field_[num_chank], pos);
        }
    }
}

int Field::getSize() const { //размер игрового поля
    return size_;
}

bool Field::getState(int row, int column) const{ //возвращаем текущее состояние клетки
    int num_chank = row * numByte(size_) + numByte(column);
    int pos = numInByte(column);
    num_chank += column % 8 == 0 && column != 0? 0: -1;
    return checkBit(field_[num_chank], pos);
}

void Field::setState(int row, int column, bool value) { //устанавливаем в нужное состояние
    int num_chank = row * numByte(size_) + numByte(column);
    int pos = numInByte(column);
    num_chank += column % 8 == 0 && column != 0? 0: -1;
    if (value) {
        setTrue(field_[num_chank], pos);
    } else {
        setFalse(field_[num_chank], pos);
    }
}
std::vector<bool> ruleToBool(const std::string &rule) { //принимает строку, представляющую правило, и преобразует её в вектор булевых 
    std::vector<bool> new_rule(8, false);               //значений, где каждый индекс соответствует состоянию 
    for (const auto &i: rule) {
        new_rule[i - '0'] = true;
    }
    return std::move(new_rule);
}

int neighbors( //считаем живых соседей
        const Field &field,
        int size,
        int row,
        int column
) {
    int count = 0;
    int maxind = size - 1;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (i != 0 || j != 0) {
                int n_row = normalize(row + i, size);
                int n_column = normalize(column + j, size);
                if (field.getState(n_row, n_column)) {
                    ++count;
                }
            }
        }
    }
    return count;
}

Game_model::Game_model(): Game_model( 
    Default_name, 
    Default_b_rule, 
    Default_s_rule, 
    Default_frame
) {}

Game_model::Game_model(
    const std::string& name,
    const std::string& brule,
    const std::string& srule, 
    const std::vector<Point>& coords
): 
    name_(name),
    b_rule_(ruleToBool(brule)),
    s_rule_(ruleToBool(srule)),
    field_1_(coords)
{};

const Field& Game_model::getField() const { //текущее поле
    return current_field_ == 1? field_1_: field_2_;
}

const std::string& Game_model::getName() const { //имя
    return name_;
}

int Game_model::getGlobIteration() const { //итерация
    return global_iteration_;
}

void Game_model::computeIteration() { //вычисляем следующий ход
    auto old_field = current_field_ == 1? &field_1_: &field_2_;
    auto new_field = current_field_ == 1? &field_2_: &field_1_;
    for (int i = 0; i < field_1_.getSize(); ++i) {
        for (int j = 0; j < field_1_.getSize(); ++j) {
            new_field->setState(i, j, false);
            int count_neigh = neighbors(*old_field, old_field->getSize(), i, j);
            if (old_field->getState(i, j) && s_rule_[count_neigh]) {
                new_field->setState(i, j, true);
            } else if (!old_field->getState(i, j) && b_rule_[count_neigh]) {
                new_field->setState(i, j, true);
            }
        }
    }
    current_field_ = current_field_ == 1? 2: 1;
    ++global_iteration_;
}