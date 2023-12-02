#pragma once

#include <ctime>
#include <string>
#include <vector>

class Minesweeper {

public:
    struct Cell {
        size_t x = 0;
        size_t y = 0;
    };

    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    GameStatus status;

    using RenderedField = std::vector<std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count);
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void NewGame(size_t width, size_t height, size_t mines_count);
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void OpenCell(const Cell& cell);
    void MarkCell(const Cell& cell);

    GameStatus GetGameStatus() const;
    time_t GetGameTime() const;

    RenderedField RenderField() const;

private:
    std::time_t time_start_ = 0;
    std::time_t game_time_ = 0;

    struct CellStatus {
        bool mine = false;
        bool flag = false;
        bool open = false;
    };

    std::vector<std::vector<CellStatus>> field_;

    void InitField(const size_t width, const size_t height);

    size_t CountMineNeighbor(const size_t y, const size_t x) const;

    size_t cells_opened_ = 0;
    size_t total_mines_ = 0;
};
