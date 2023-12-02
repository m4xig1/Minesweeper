#include "minesweeper.h"

#include <iostream>
#include <queue>
#include <random>

void Minesweeper::InitField(const size_t width, const size_t height) {  // создаем таблицу нужных размеров
    field_.reserve(height);
    for (size_t i = 0; i < height; ++i) {
        field_.push_back(std::vector<CellStatus>(width));
    }
}

void Minesweeper::NewGame(size_t width, size_t height, size_t mines_count) {
    status = GameStatus::NOT_STARTED;
    field_.clear();
    cells_opened_ = 0;
    total_mines_ = std::min(mines_count, width * height);
    InitField(width, height);
    std::vector<std::pair<size_t, size_t>> cells;
    cells.reserve(width * height);
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            cells.push_back(std::make_pair(i, j));  // вектор с точками
        }
    }
    // шафлим все клетки, затем выбираем из них первые mines_count
    std::shuffle(cells.begin(), cells.end(), std::mt19937(time(0)));

    for (size_t i = 0; i < mines_count; ++i) {
        field_[cells[i].first][cells[i].second].mine = true;  // [i][j]
    }
}

void Minesweeper::NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    status = GameStatus::NOT_STARTED;
    field_.clear();
    cells_opened_ = 0;
    total_mines_ = 0;
    InitField(width, height);
    for (const auto& [x, y] : cells_with_mines) {
        if (!field_[y][x].mine) {
            ++total_mines_;  // увеличиваем количество мин, только если еще такой мины не было
        }
        field_[y][x].mine = true;
    }
}

Minesweeper::Minesweeper(size_t width, size_t height, size_t mines_count) {
    NewGame(width, height, mines_count);
}

Minesweeper::Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    NewGame(width, height, cells_with_mines);
}

// возвращает количество соседей ячейки, которые являются минами
size_t Minesweeper::CountMineNeighbor(const size_t y, const size_t x) const {
    size_t count_mines = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            // проверка на границы и мину
            if (x + dx >= 0 && x + dx < field_[0].size() && y + dy >= 0 && y + dy < field_.size() &&
                field_[y + dy][x + dx].mine) {
                ++count_mines;
            }
        }
    }
    return count_mines;
};

// возможно, надо обработать случай, когда все заполнено минами
void Minesweeper::OpenCell(const Cell& cell) {
    if (status != GameStatus::VICTORY && status != GameStatus::DEFEAT) {
        if (status == GameStatus::NOT_STARTED) {
            status = GameStatus::IN_PROGRESS;
            time_start_ = time(0);  // записываем время первого нажатия на кнопку
        }
        if (status == GameStatus::IN_PROGRESS) {
            if (!field_[cell.y][cell.x].flag) {     // если клетка без флага
                if (field_[cell.y][cell.x].mine) {  // попались на мину
                    status = GameStatus::DEFEAT;
                    game_time_ = time(0) - time_start_;
                    // открываем все клетки
                    for (size_t i = 0; i < field_.size(); ++i) {
                        for (size_t j = 0; j < field_[0].size(); ++j) {
                            field_[i][j].open = true;
                        }
                    }
                } else {
                    std::queue<std::pair<size_t, size_t>> cells_que;  // bfs для клеток
                    cells_que.push(std::make_pair(cell.x, cell.y));
                    while (!cells_que.empty()) {
                        const size_t x = cells_que.front().first;  // (x, y) на вход. [y][x] - в массив
                        const size_t y = cells_que.front().second;
                        cells_que.pop();
                        if (field_[y][x].open) {  // в массив могут быть записаны ячейки, которые мы уже открыли
                            continue;
                        }
                        field_[y][x].open = true;
                        ++cells_opened_;

                        // если в соседних клетках нет мин, то записываем в очередь всех еще не открытых соседей без
                        // флагов
                        if (CountMineNeighbor(y, x) == 0) {
                            // проверим, не выходят ли ячейки за пределы массива
                            // добавляем, если: в массиве, нет флага, не открыта:
                            for (int dx = -1; dx <= 1; ++dx) {
                                for (int dy = -1; dy <= 1; ++dy) {
                                    if (dx == 0 && dy == 0) {
                                        continue;
                                    }
                                    // проверка условий
                                    if (x + dx >= 0 && x + dx < field_[0].size() && y + dy >= 0 &&
                                        y + dy < field_.size() && !field_[y + dy][x + dx].flag &&
                                        !field_[y + dy][x + dx].open) {
                                        cells_que.push(std::make_pair(x + dx, y + dy));
                                    }
                                }
                            }
                        }
                    }
                    // условие победы
                    if (cells_opened_ == field_.size() * field_[0].size() - total_mines_) {
                        game_time_ = time(0) - time_start_;
                        status = GameStatus::VICTORY;
                    }
                }
            }
        }
    }
};

void Minesweeper::MarkCell(const Cell& cell) {
    if (status != GameStatus::VICTORY && status != GameStatus::DEFEAT) {
        if (status == GameStatus::NOT_STARTED) {
            status = GameStatus::IN_PROGRESS;
            time_start_ = time(0);
        }
        if (status == GameStatus::IN_PROGRESS) {
            if (!field_[cell.y][cell.x].open) {
                field_[cell.y][cell.x].flag = !field_[cell.y][cell.x].flag;  // меняем значение на ячейке с флагом
            }
        }
    }
};

Minesweeper::GameStatus Minesweeper::GetGameStatus() const {
    return status;
};

time_t Minesweeper::GetGameTime() const {
    if (status == GameStatus::VICTORY || status == GameStatus::DEFEAT) {
        return game_time_;
    } else if (status == GameStatus::NOT_STARTED) {
        return 0;
    }
    return time(0) - time_start_;
};

Minesweeper::RenderedField Minesweeper::RenderField() const {
    RenderedField field;
    for (size_t i = 0; i < field_.size(); ++i) {
        std::string field_line;
        field_line.reserve(field_[0].size());
        for (size_t j = 0; j < field_[0].size(); ++j) {
            if (!field_[i][j].open) {
                if (field_[i][j].flag) {
                    field_line += "?";
                } else {
                    field_line += "-";
                }
            } else if (field_[i][j].mine) {
                field_line += "*";
            } else {
                size_t mines_around = CountMineNeighbor(i, j);
                if (mines_around == 0) {  // [y][x] = [i][j]
                    field_line += '.';
                } else {
                    field_line += std::to_string(mines_around);
                }
            }
        }
        field.push_back(std::move(field_line));
    }
    return field;
};
