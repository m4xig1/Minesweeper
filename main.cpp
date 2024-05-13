#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "minesweeper.h"


void PrintField(const Minesweeper::RenderedField& field) {
    for (const auto& row : field) {
        std::cout << row << std::endl;
    }
}

int main() {
    size_t width = 10; // Adjust these values as per your preference
    size_t height = 10;
    size_t mines_count = 15;

    Minesweeper game(width, height, mines_count);

    bool game_over = false;

    while (!game_over) {
        system("clear"); // For Linux/Mac
        // system("cls"); // For Windows

        Minesweeper::RenderedField field = game.RenderField();
        PrintField(field);

        std::cout << "Enter cell coordinates (x y): ";
        size_t x, y;
        std::cin >> x >> y;

        Minesweeper::Cell cell{x, y};
        game.OpenCell(cell);

        Minesweeper::GameStatus status = game.GetGameStatus();
        if (status == Minesweeper::GameStatus::VICTORY) {
            std::cout << "Congratulations! You won!" << std::endl;
            game_over = true;
        } else if (status == Minesweeper::GameStatus::DEFEAT) {
            std::cout << "Game over! You hit a mine!" << std::endl;
            game_over = true;
        }
    }

    return 0;
}
